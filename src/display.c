/** @file display.c */
#include <stdio.h>

#include <exec/interrupts.h>
#include <hardware/intbits.h>

#include <hardware/custom.h>
#include <graphics/gfxbase.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>

#include <ratr0/debug_utils.h>
#include <ratr0/engine.h>
#include <ratr0/input.h>
#include <ratr0/timers.h>

#include <ratr0/memory.h>
#include <ratr0/datastructs/bitset.h>
#include <ratr0/resources.h>

#include <ratr0/hw_registers.h>
#include <ratr0/display.h>
#include <ratr0/sprites.h>
#include <ratr0/blitter.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("DISPLAY", __VA_ARGS__)

static struct Ratr0RenderingSystem rendering_system;

/**
 * Encapsulate the Amiga hardware specifics of display aspects here.
 * The RATR0 rendering engine is based on a pipeline of graphics objects.
 * This is essentially a queue of drawing operations:
 *  - sprites (set to copper list)
 *  - blitter operations: a per-frame-queue, that is operated by Blitter interrupts
 *  - scrolling if needed
 */
/*
 * We assume these to exist.
 */
extern struct GfxBase *GfxBase;
extern struct Custom custom;
static Ratr0Engine *engine;

/**
 * current active copper list
 */
struct Ratr0CopperListInfo *current_copper_info;
UINT16 *current_coplist;
int current_coplist_size;

// Double buffer management
#define MAX_BUFFERS (2)
static void _update_front_buffers(UINT16 coplist[], int num_words,
                                  struct Ratr0CopperListInfo *info);

// For our interrupt handlers
static struct Interrupt vbint;
UINT8 frames_elapsed;

// A bitset for each buffer, 10x32 = 320 rectangles each
// representing 20x16 rectangles of 16x16 pixels on a 320x256 frame
// We can map a rectangle (x,y) to a bit index by using
// p_i = y * 20 + x and y = p_i / 20, x = p_i % 20 in row major order

// If we can for example use a base 2 dimension, like the height in this case
// we can use column major order and compute
// p_i = x * 20 + y and x = p_i % 16, x = p_i % 16 in column major order
// which can be faster since we can use bitwise operations instead of
// divisions
#define BITSET_SIZE (10)
#define BITSET_INDEX(x, y) (y * 20 + x)
#define BITSET_X(idx) (idx % 20)
#define BITSET_Y(idx) (idx / 20)

/**
 * Captures the properties of our display, which can be either 1 or 2
 * playfields, each with up to 2 buffers
 */
struct Playfield {
    struct Ratr0DisplayBuffer display_buffer[MAX_BUFFERS];
    UINT16 back_buffer, front_buffer;
    UINT32 bitset_arr[MAX_BUFFERS][BITSET_SIZE];
    UINT32 display_buffer_size;
};
static struct Playfield playfields[MAX_PLAYFIELDS];

#define HW_SPRITE_TABLE_SIZE (20)
// Sprite and bob tables can be in Fastmem
struct Ratr0HWSprite hw_sprite_table[HW_SPRITE_TABLE_SIZE];
UINT16 next_hw_sprite = 0;
struct Ratr0Bob bob_table[20];
UINT16 next_bob = 0;

void ratr0_display_add_dirty_rectangle(UINT16 playfield_num, UINT16 x, UINT16 y)
{
    struct Playfield *playfield = &playfields[playfield_num];
    // Add the rectangles to both buffers
    ratr0_bitset_insert(playfield->bitset_arr[playfield->front_buffer],
                        BITSET_SIZE, BITSET_INDEX(x, y));
    ratr0_bitset_insert(playfield->bitset_arr[playfield->back_buffer],
                        BITSET_SIZE, BITSET_INDEX(x, y));
}

static void (*_process_rect)(struct Ratr0DisplayBuffer *, UINT16 x, UINT16 y);

void process_bit(UINT16 index, void *userdata)
{
    UINT16 x = BITSET_X(index) << 4;  // * 16
    UINT16 y = BITSET_Y(index) << 4;
    _process_rect((struct Ratr0DisplayBuffer *) userdata, x, y);
}

void ratr0_display_process_dirty_rectangles(void (*process_dirty_rect)(struct Ratr0DisplayBuffer *, UINT16, UINT16))
{
    for (int playfield_num = 0; playfield_num < display_info.num_playfields;
         playfield_num++) {
        struct Playfield *playfield = &playfields[playfield_num];
        int backbuffer_num = playfield->back_buffer;
        struct Ratr0DisplayBuffer *backbuffer =
            &playfield->display_buffer[backbuffer_num];

        // Establish the rect processing function
        _process_rect = process_dirty_rect;
        // 1. Restore background using the dirty tile set
        ratr0_bitset_iterate(playfield->bitset_arr[backbuffer_num],
                             BITSET_SIZE, &process_bit, backbuffer);
        ratr0_bitset_clear(playfield->bitset_arr[backbuffer_num],
                           10); // clear to reset
    }
}

// Swap back and front buffers
void ratr0_display_swap_buffers(void)
{
    for (int playfield_num = 0; playfield_num < display_info.num_playfields;
         playfield_num++) {
        struct Playfield *playfield = &playfields[playfield_num];

        //  just the first buffer single buffering -> next playfield
        if (display_info.playfield[playfield_num].num_buffers == 1) {
            continue;
        }

        // 1. swap front and back indexes
        int tmp = playfield->front_buffer;
        playfield->front_buffer = playfield->back_buffer;
        playfield->back_buffer = tmp;
    }
    // 2. set new front buffer to copper list
    _update_front_buffers(current_coplist, current_coplist_size,
                          current_copper_info);
}

// Our vertical blank server only implements a simple frame counter
// we can use for things like timers etc.
// Note: What does not seem to work is swapping the display buffers
// ----- For some reason it triggers some kind of race condition that
//       interferes with the blitting
void set_zero_flag(void) = "\tmoveq.l\t#0,d0\n";
void VertBServer()
{
    frames_elapsed++;
    ratr0_input_update();
    ratr0_timers_tick();
    set_zero_flag();
}

struct Ratr0DisplayInfo display_info;

/* This is a bit of a trick: I pre-allocate dummy sprite data */
UINT16 __chip NULL_SPRITE_DATA[] = {
    0x0000, 0x0000,
    0x0000, 0x0000
};

struct Ratr0HWSprite NULL_HW_SPRITE = {
    {
        {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0},
        {0, {0,0,0,0, 0,0,0,0}, 0, 0, 0, 0, 0}
    },
    FALSE, NULL_SPRITE_DATA
};

/**
 * We need to adjust the BPLCONx and BPLxMOD values after changing the
 * display mode. This function looks at display_info and will
 * will set the BPLCON0 and BPLxMOD registers accordingly depending
 * on dual playfield or single playfield modes
 */
static void _set_display_mode(UINT16 coplist[],
                             struct Ratr0CopperListInfo *info)
{
    // width *needs* to be a multiple of 16 because Amiga playfield hardware
    // operates on word boundaries
    BOOL is_dual_playfield = display_info.num_playfields == 2;
    UINT16 dpf_flag = is_dual_playfield ? 0x400 : 0;
    UINT16 playfield0_width = display_info.playfield[0].buffer_width;
    UINT16 playfield1_width = display_info.playfield[1].buffer_width;
    UINT16 playfield0_depth = display_info.playfield[0].depth;
    UINT16 playfield1_depth = display_info.playfield[1].depth;

    UINT16 playfield_total_depth = is_dual_playfield ?
        playfield0_depth + playfield1_depth : playfield0_depth;

    UINT16 screenrow_bytes0 = playfield0_width / 8;
    UINT16 screenrow_bytes1 = playfield1_width / 8;
    UINT16 bpl1mod = screenrow_bytes0 * (playfield0_depth - 1);
    UINT16 bpl2mod = is_dual_playfield ?
        screenrow_bytes1 * (playfield1_depth - 1) : bpl1mod;

    UINT16 bplcon0_value = (playfield_total_depth << 12) | dpf_flag
        | 0x200;
    coplist[info->bplcon0_index] = bplcon0_value;
    coplist[info->bpl1mod_index] = bpl1mod;
    coplist[info->bpl1mod_index + 2] = bpl2mod;
}

/**
 * Set the bitplane pointers in the copper list to the specified display buffer
 * It also adjusts BPLCONx and BPLxMOD.
 */
struct Ratr0DisplayBuffer *ratr0_display_get_front_buffer(UINT16 playfield_num)
{
    struct Playfield *playfield = &playfields[playfield_num];
    return &playfield->display_buffer[playfield->front_buffer];
}

struct Ratr0DisplayBuffer *ratr0_display_get_back_buffer(UINT16 playfield_num)
{
    struct Playfield *playfield = &playfields[playfield_num];
    return &playfield->display_buffer[playfield->back_buffer];
}

BOOL ratr0_display_blit_surface_to_buffers(struct Ratr0Surface *surface,
                                           UINT16 playfield_num,
                                           UINT16 dstx, UINT16 dsty)
{
    struct Ratr0Surface *back_buffer, *front_buffer;
    front_buffer = &ratr0_display_get_front_buffer(playfield_num)->surface;
    back_buffer = &ratr0_display_get_back_buffer(playfield_num)->surface;
    OwnBlitter();
    ratr0_blit_rect_simple(front_buffer, surface, dstx, dsty, 0, 0,
                           surface->width, surface->height);
    ratr0_blit_rect_simple(back_buffer, surface, dstx, dsty, 0, 0,
                           surface->width, surface->height);
    DisownBlitter();
    return TRUE;
}

/**
 * Private function to apply the new front buffers to the copper list.
 * This sets the correct BPLxPTH/BPLxPTL pointers.
 * TODO: if we are working with dual playfield, we need to take
 * the pointers from both playfields' bitmaps
 */
static void _update_front_buffers(UINT16 coplist[], int num_words,
                                 struct Ratr0CopperListInfo *info)
{
    // TODO: we need to process all playfields. That means
    // if we have dual playfields, we need to set odd and even playfields
    //   * Playfield 0 uses bitplanes 1, 3, 5
    //   * Playfield 1 uses bitplanes 2, 4, 6
    // Actually, if one playfield does not use double buffering,
    // we don't even need to update the buffer pointers for the playfield
    // more than once. Right now, we won't bother, it might actually not matter

    if (display_info.num_playfields ==  1) {
        struct Playfield *playfield = &playfields[0];
        struct Ratr0Surface *s = &playfield->display_buffer[playfield->front_buffer].surface;
        UINT16 screenrow_bytes = s->width / 8;
        UINT32 plane = (UINT32) s->buffer;
        UINT32 clidx = info->bpl1pth_index;

        for (int i = 0; i < s->depth; i++) {
            coplist[clidx] = (plane >> 16) & 0xffff;
            coplist[clidx + 2] = plane & 0xffff;
            plane += screenrow_bytes;
            clidx += 4;
        }
    } else {
        struct Playfield *playfield1 = &playfields[0];
        struct Ratr0Surface *s1 =
            &playfield1->display_buffer[playfield1->front_buffer].surface;

        // dual playfield mode
        UINT16 screenrow_bytes = s1->width / 8;
        UINT32 plane = (UINT32) s1->buffer;
        UINT32 clidx = info->bpl1pth_index;

        for (int i = 0; i < s1->depth; i++) {
            coplist[clidx] = (plane >> 16) & 0xffff;
            coplist[clidx + 2] = plane & 0xffff;
            plane += screenrow_bytes;
            clidx += 8;
        }

        struct Playfield *playfield2 = &playfields[1];
        struct Ratr0Surface *s2 =
            &playfield2->display_buffer[playfield2->front_buffer].surface;
        screenrow_bytes = s2->width / 8;
        plane = (UINT32) s2->buffer;
        clidx = info->bpl1pth_index + 4;

        for (int i = 0; i < s2->depth; i++) {
            coplist[clidx] = (plane >> 16) & 0xffff;
            coplist[clidx + 2] = plane & 0xffff;
            plane += screenrow_bytes;
            clidx += 8;
        }
    }
}

/**
 * Initialize the given copper list with the essential information about
 * the display.
 *
 * @param coplist pointer to copper list
 * @param num_words length of list in words
 * @param info copper list index info
 */
void _ratr0_display_init_copperlist(UINT16 coplist[], int num_words,
                                    struct Ratr0CopperListInfo *info)
{
    // set up the display and DMA windows (16 bytes)
    // 1. look at the viewport size to build display window and DMA start
    // We support a width of 288 and a width of 320
    if (display_info.vp_width == 288) {
        //PRINT_DEBUG("288 DDFSTRT");
        coplist[info->ddfstrt_index] = DDFSTRT_VALUE_288;
        coplist[info->ddfstop_index] = DDFSTOP_VALUE_288;
        coplist[info->diwstrt_index] = DIWSTRT_VALUE_288;
        if (display_info.is_pal) {
            coplist[info->diwstop_index] = DIWSTOP_VALUE_PAL_288;
        } else {
            coplist[info->diwstop_index] = DIWSTOP_VALUE_NTSC_288;
        }
    } else {
        //PRINT_DEBUG("320 DDFSTRT");
        coplist[info->ddfstrt_index] = DDFSTRT_VALUE_320;
        coplist[info->ddfstop_index] = DDFSTOP_VALUE_320;
        if (display_info.is_pal) {
            coplist[info->diwstop_index] = DIWSTOP_VALUE_PAL_320;
        } else {
            coplist[info->diwstop_index] = DIWSTOP_VALUE_NTSC_320;
        }
    }

    // 2. Initialize the sprites with the NULL address (8x8 = 64 bytes)
    for (int i = 0; i < 8; i++) {
        ratr0_display_set_sprite(coplist, num_words, info,
                                 i, NULL_SPRITE_DATA);
    }

    // 3. establish the display mode in the copper list
    _set_display_mode(coplist, info);

    // Establish the playfield display buffers
    _update_front_buffers(coplist, num_words, info);
}

static void build_display_buffer(UINT16 num_playfields,
                                 struct Ratr0PlayfieldInfo pf_infos[])
{
    for (int playfield_num = 0; playfield_num < num_playfields;
         playfield_num++) {

        playfields[playfield_num].display_buffer_size =
            pf_infos[playfield_num].buffer_width / 8 *
            pf_infos[playfield_num].buffer_height *
            pf_infos[playfield_num].depth;

        for (int j = 0; j < pf_infos[playfield_num].num_buffers; j++) {
            playfields[playfield_num].display_buffer[j].buffernum = j;
            playfields[playfield_num].display_buffer[j].surface.width =
                pf_infos[playfield_num].buffer_width;
            playfields[playfield_num].display_buffer[j].surface.height =
                pf_infos[playfield_num].buffer_height;
            playfields[playfield_num].display_buffer[j].surface.depth =
                pf_infos[playfield_num].depth;
            playfields[playfield_num].display_buffer[j].surface.is_interleaved = TRUE;

            // TODO
            // display buffer memory is allocated directly from the OS
            // since we haven't ironed out our memory allocation strategy
            // which currently might lead to memory exhaustion. But we
            // should actually be able to use the RATR0 allocator
            playfields[playfield_num].display_buffer[j].surface.buffer =
                (void *) AllocMem(playfields[playfield_num].display_buffer_size,
                                  MEMF_CHIP|MEMF_CLEAR);
            if (playfields[playfield_num].display_buffer[j].surface.buffer == NULL) {
                PRINT_DEBUG("ERROR: can't allocate memory for display buffers !");
                break;
            }
        }
    }

    // don't forget to set the number of playfields
    display_info.num_playfields = num_playfields;
}

static void free_display_buffer(void)
{
    for (int playfield_num = 0; playfield_num < display_info.num_playfields; playfield_num++) {
        for (int j = 0; j < display_info.playfield[playfield_num].num_buffers; j++) {
            if (playfields[playfield_num].display_buffer[j].surface.buffer != NULL) {
                FreeMem(playfields[playfield_num].display_buffer[j].surface.buffer,
                        playfields[playfield_num].display_buffer_size);
            }
        }
    }
}

// Blitter and sprite queues here

void _install_interrupts(void)
{
    // Interrupt server for Vertical Blank, only PORTS, COPER, VERTB, EXTER and NMI
    // can be serviced throught an interrupt server because they are chained
    vbint.is_Node.ln_Type = NT_INTERRUPT;
    vbint.is_Node.ln_Pri = -60;
    vbint.is_Node.ln_Name = "vbinter";
    vbint.is_Data = (APTR) NULL;  // unused
    vbint.is_Code = VertBServer;
    AddIntServer(INTB_VERTB, &vbint);
}

void _uninstall_interrupts(void)
{
    // Remove vertical blank handler
    RemIntServer(INTB_VERTB, &vbint);
}

struct Ratr0RenderingSystem *ratr0_display_startup(Ratr0Engine *eng)
{
    engine = eng;
    rendering_system.shutdown = &ratr0_display_shutdown;
    frames_elapsed = 0;

    ratr0_sprites_startup(eng);
    ratr0_blitter_startup(eng);

    LoadView(NULL);  // clear display, reset hardware registers
    WaitTOF();       // 2 WaitTOFs to wait for 1. long frame and
    WaitTOF();       // 2. short frame copper lists to finish (if interlaced)

    display_info.is_pal = (((struct GfxBase *) GfxBase)->DisplayFlags & PAL) == PAL;

    _install_interrupts();

    // initialize display buffers and display info
    for (int i = 0; i < MAX_PLAYFIELDS; i++) {
        playfields[i].display_buffer_size = 0;
        display_info.playfield[i].num_buffers = 0;
        display_info.playfield[i].buffer_width = 0;
        display_info.playfield[i].buffer_height = 0;
        display_info.playfield[i].depth = 0;
    }

    // Object management initialization
    next_hw_sprite = next_bob = 0;
    PRINT_DEBUG("Startup finished");
    return &rendering_system;
}

void ratr0_display_init_buffers(UINT16 vp_width, UINT16 vp_height,
                                UINT16 num_playfields,
                                struct Ratr0PlayfieldInfo pf_infos[])
{
    for (int playfield_num = 0; playfield_num < num_playfields;
         playfield_num++) {
        struct Playfield *playfield = &playfields[playfield_num];
        struct Ratr0PlayfieldInfo *pfinfo = &display_info.playfield[playfield_num];
        struct Ratr0PlayfieldInfo *pfinit = &pf_infos[playfield_num];

        // optimization: if the memory requirement is actually smaller,
        // we can reuse the display buffer memory instead of freeing it
        // and only rebuild if we need a larger buffer
        if (pfinfo->buffer_width != pfinit->buffer_width ||
            pfinfo->buffer_height != pfinit->buffer_height ||
            pfinfo->depth != pfinit->depth ||
            pfinfo->num_buffers != pfinit->num_buffers) {

            display_info.vp_width = vp_width;
            display_info.vp_height = vp_height;

            pfinfo->buffer_width = pfinit->buffer_width;
            pfinfo->buffer_height = pfinit->buffer_height;
            pfinfo->depth = pfinit->depth;
            pfinfo->num_buffers = pfinit->num_buffers;

            if (pfinit->num_buffers == 1) {
                playfield->front_buffer = playfield->back_buffer = 0;
            } else {
                playfield->front_buffer = 0;
                playfield->back_buffer = 1;
            }
            // Rebuild the display buffer if the new one would be bigger
            int new_display_buffer_size = pfinit->buffer_width / 8 *
                pfinit->buffer_height * pfinit->depth;

            if (new_display_buffer_size > playfield->display_buffer_size) {
                free_display_buffer();
                build_display_buffer(num_playfields, pf_infos);
            }
            ratr0_bitset_clear(playfield->bitset_arr[playfield->back_buffer],
                               BITSET_SIZE);
            ratr0_bitset_clear(playfield->bitset_arr[playfield->front_buffer],
                               BITSET_SIZE);
        }
    }
 }

void ratr0_display_set_copperlist(UINT16 *copperlist, int size,
                                  struct Ratr0CopperListInfo *info)
{
    _ratr0_display_init_copperlist(copperlist, size, info);
    WaitTOF();
    current_coplist = copperlist;
    current_copper_info = info;
    current_coplist_size = size;
    custom.cop1lc = (ULONG) copperlist;
}

void ratr0_display_shutdown(void)
{
    free_display_buffer();
    _uninstall_interrupts();
    ratr0_sprites_shutdown();

    // Restore the Workbench display by restoring the original copper list
    LoadView(((struct GfxBase *) GfxBase)->ActiView);
    WaitTOF();
    WaitTOF();
    custom.cop1lc = (ULONG) ((struct GfxBase *) GfxBase)->copinit;
    RethinkDisplay();
}

void ratr0_display_set_palette(UINT16 *colors, UINT8 num_colors, UINT8 offset)
{
    for (int i = 0; i < num_colors; i++) {
        current_coplist[current_copper_info->color00_index + (i + offset) * 2] = colors[i];
    }
}

void ratr0_display_set_sprite(UINT16 *coplist, int size,
                              struct Ratr0CopperListInfo *copinfo,
                              int sprite_num, UINT16 *data)
{
    int spr_idx = copinfo->spr0pth_index + 4 * sprite_num;
    coplist[spr_idx] = ((UINT32) data >> 16) & 0xffff;
    coplist[spr_idx + 2] = (UINT32) data & 0xffff;
}

// OBJECT MANAGEMENT
struct Ratr0HWSprite *ratr0_create_sprite(struct Ratr0TileSheet *tilesheet,
                                          UINT8 frames[], UINT8 num_frames, UINT8 speed)
{
    // Note we can only work within the Amiga hardware sprite limitations
    // 1. Reserve memory from engine
    // 2. Convert into sprite data structure and store into allocated memory
    // 3. Return the initialized object
    UINT16 *sprite_data = ratr0_make_sprite_data(tilesheet, frames, num_frames);
    struct Ratr0HWSprite *result = &hw_sprite_table[next_hw_sprite++];
    result->sprite_data = sprite_data;
    // store sprite information
    return result;
}

struct Ratr0HWSprite *ratr0_create_sprite_from_sprite_sheet(struct Ratr0SpriteSheet *sheet,
                                                            UINT8 speed, UINT8 loop_type)
{
    struct Ratr0HWSprite *result = &hw_sprite_table[next_hw_sprite++];
    // Data and frame information
    result->sprite_data = (UINT16 *) ratr0_memory_block_address(sheet->h_imgdata);

    // remember the attachment state here
    result->is_attached = ((result->sprite_data[1] & 0x80) == 0x80);

    // copy size and color information
    result->base_obj.bounds.x = 0;
    result->base_obj.bounds.y = 0;
    result->base_obj.bounds.width = 16;
    result->base_obj.bounds.height = (int) result->sprite_data[0];
    result->base_obj.anim_frames.num_frames = sheet->header.num_sprites;
    result->base_obj.anim_frames.current_frame_idx = 0;
    result->base_obj.anim_frames.current_tick = 0;
    result->base_obj.translate.x = 0;
    result->base_obj.translate.y = 0;
    // Copy frames
    for (int i = 0; i < sheet->header.num_sprites; i++) {
        result->base_obj.anim_frames.frames[i] = i;
    }
    // loop type and speed could possibly be part of the sprite sheet
    result->base_obj.anim_frames.loop_type = loop_type;
    result->base_obj.anim_frames.speed = speed;
    result->base_obj.anim_frames.loop_dir = 1;
    return result;
}

struct Ratr0HWSprite *ratr0_create_sprite_from_sprite_sheet_frame(struct Ratr0SpriteSheet *sheet, int framenum)
{
    struct Ratr0HWSprite *result = &hw_sprite_table[next_hw_sprite++];
    // Data and frame information
    result->sprite_data = (UINT16 *) ratr0_memory_block_address(sheet->h_imgdata);

    // remember the attachment state here
    result->is_attached = ((result->sprite_data[1] & 0x80) == 0x80);

    // copy size and color information
    result->base_obj.bounds.x = 0;
    result->base_obj.bounds.y = 0;
    result->base_obj.bounds.width = 16;
    result->base_obj.bounds.height = (int) result->sprite_data[0];
    result->base_obj.anim_frames.num_frames = 1;
    result->base_obj.anim_frames.current_frame_idx = 0;
    result->base_obj.anim_frames.current_tick = 0;
    result->base_obj.translate.x = 0;
    result->base_obj.translate.y = 0;
    result->base_obj.anim_frames.frames[0] = framenum;
    // loop type and speed could possibly be part of the sprite sheet
    result->base_obj.anim_frames.loop_type = RATR0_ANIM_LOOP_TYPE_NONE;
    result->base_obj.anim_frames.speed = 1;
    result->base_obj.anim_frames.loop_dir = 1;
    return result;
}


struct Ratr0Bob *ratr0_create_bob(struct Ratr0TileSheet *tilesheet,
                                  UINT8 frames[], UINT8 num_frames,
                                  UINT8 speed)
{
    if (num_frames > RATR0_MAX_ANIM_FRAMES) {
        PRINT_DEBUG("Can't create BOB with more than %d animation frames !", RATR0_MAX_ANIM_FRAMES);
        return NULL;
    }
    struct Ratr0Bob *result = &bob_table[next_bob++];
    result->tilesheet = tilesheet;

    result->base_obj.anim_frames.num_frames = num_frames;
    result->base_obj.anim_frames.current_frame_idx = 0;
    result->base_obj.anim_frames.current_tick = 0;
    result->base_obj.anim_frames.speed = speed;
    for (int i = 0; i < num_frames; i++) {
        result->base_obj.anim_frames.frames[i] = frames[i];
    }

    result->base_obj.bounds.x = 0;
    result->base_obj.bounds.y = 0;
    result->base_obj.bounds.width = tilesheet->header.tile_width;
    result->base_obj.bounds.height = tilesheet->header.tile_height;

    result->base_obj.translate.x = 0;
    result->base_obj.translate.y = 0;

    // By default, set the collision box to the same size
    result->base_obj.collision_box.x = 0;
    result->base_obj.collision_box.y = 0;
    result->base_obj.collision_box.width = tilesheet->header.tile_width;
    result->base_obj.collision_box.height = tilesheet->header.tile_height;

    return result;
}

void ratr0_dump_copperlist(UINT16 *copperlist, int len, const char *path)
{
    FILE *fp = fopen(path, "w");
    if (fp) {
        fprintf(fp, "UINT16 coplist[] = {\n\t");
        for (int i = 0; i < len; i++) {
            if (i > 0) fprintf(fp, ", ");
            fprintf(fp, "0x%03x", copperlist[i]);
        }
        fprintf(fp, "};\n");
        fclose(fp);
    } else {
#ifdef DEBUG
        fprintf(debug_fp, "could not open '%s'\n", path);
        fflush(debug_fp);
#endif
    }
}
