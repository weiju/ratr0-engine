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

#include <ratr0/memory.h>
#include <ratr0/datastructs/bitset.h>
#include <ratr0/resources.h>

#include <ratr0/hw_registers.h>
#include <ratr0/display.h>
#include <ratr0/sprites.h>
#include <ratr0/blitter.h>

#include "default_copper.h"

/** \brief default copper list indexes */
struct Ratr0CopperListInfo DEFAULT_COPPER_INFO = {
    3, 5, 7, 9,
    DEFAULT_COPPER_BPLCON0_INDEX, DEFAULT_COPPER_BPL1MOD_INDEX,
    DEFAULT_COPPER_BPL1PTH_INDEX,
    DEFAULT_COPPER_SPR0PTH_INDEX, DEFAULT_COPPER_COLOR00_INDEX
};

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[32mDISPLAY\033[0m", __VA_ARGS__)

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
static void set_display_surface(UINT16 coplist[], int num_words,
                                struct Ratr0CopperListInfo *info,
                                struct Ratr0Surface *s);

static struct Ratr0DisplayBuffer display_buffer[MAX_BUFFERS];
UINT16 ratr0_back_buffer = 1;
UINT16 ratr0_front_buffer = 0;

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
UINT32 bitset_arr[2][BITSET_SIZE];

// Sprite and bob tables can be in Fastmem
struct Ratr0HWSprite hw_sprite_table[20];
UINT16 next_hw_sprite = 0;
struct Ratr0Bob bob_table[20];
UINT16 next_bob = 0;

void add_dirty_rectangle(UINT16 x, UINT16 y)
{
    // Add the rectangles to both buffers
    ratr0_bitset_insert(bitset_arr[ratr0_front_buffer], BITSET_SIZE, BITSET_INDEX(x, y));
    ratr0_bitset_insert(bitset_arr[ratr0_back_buffer], BITSET_SIZE, BITSET_INDEX(x, y));
}

static void (*_process_rect)(UINT16 x, UINT16 y);

void process_bit(UINT16 index)
{
    UINT16 x = BITSET_X(index) << 4;  // * 16
    UINT16 y = BITSET_Y(index) << 4;
    _process_rect(x, y);
}

void process_dirty_rectangles(void (*process_dirty_rect)(UINT16 x, UINT16 y))
{
    // Establish the rect processing function
    _process_rect = process_dirty_rect;
    // 1. Restore background using the dirty tile set
    ratr0_bitset_iterate(bitset_arr[ratr0_back_buffer], BITSET_SIZE, &process_bit);
    ratr0_bitset_clear(bitset_arr[ratr0_back_buffer], 10); // clear to reset
}

// Swap back and front buffers
void ratr0_display_swap_buffers(void)
{
    // 1. swap front and back indexes
    int tmp = ratr0_front_buffer;
    ratr0_front_buffer = ratr0_back_buffer;
    ratr0_back_buffer = tmp;
    // 2. set new front buffer to copper list
    set_display_surface(current_coplist, current_coplist_size,
                        current_copper_info,
                        &display_buffer[ratr0_front_buffer].surface);
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
    engine->input_system->update();
    set_zero_flag();
}

struct Ratr0DisplayInfo display_info;

/* This is a bit of a trick: I pre-allocate dummy sprite data */
UINT16 __chip NULL_SPRITE_DATA[] = {
    0x0000, 0x0000,
    0x0000, 0x0000
};

/**
 * We need to adjust the BPLCONx and BPLxMOD values after changing the
 * display mode.
 * Note: bplmod is currently the same for BPL1MOD and BPL2MOD, so no
 * dual playfield as for now
 */
static void set_display_mode(UINT16 coplist[],
                             struct Ratr0CopperListInfo *info,
                             UINT16 width, UINT8 num_bitplanes)
{
    // width *needs* to be a multiple of 16 because Amiga playfield hardware operates
    // on word boundaries
    UINT16 screenrow_bytes = width / 8;
    UINT16 bplmod = screenrow_bytes * (num_bitplanes - 1);
    UINT16 bplcon0_value = (num_bitplanes << 12) | 0x200;
    /*
    PRINT_DEBUG("screenrow_bytes: %d bpl1mod: %d num_bitplanes: %d bplcon0: %04x",
                (int) screenrow_bytes, (int) bplmod, (int) num_bitplanes, (int) bplcon0_value);
    */
    coplist[info->bplcon0_index] = bplcon0_value;
    coplist[info->bpl1mod_index] = bplmod;
    coplist[info->bpl1mod_index + 2] = bplmod;
}

/**
 * Set the bitplane pointers in the copper list to the specified display buffer
 * It also adjusts BPLCONx and BPLxMOD.
 */
struct Ratr0DisplayBuffer *ratr0_get_front_buffer(void)
{
    return &display_buffer[ratr0_front_buffer];
}
struct Ratr0DisplayBuffer *ratr0_get_back_buffer(void)
{
    return &display_buffer[ratr0_back_buffer];
}

/**
 * Private function to apply the render context to the copper list
 */
static void set_display_surface(UINT16 coplist[], int num_words,
                                struct Ratr0CopperListInfo *info,
                                struct Ratr0Surface *s)
{
    UINT16 screenrow_bytes = s->width / 8;
    UINT32 plane = (UINT32) s->buffer;
    UINT32 clidx = info->bpl1pth_index;

    for (int i = 0; i < s->depth; i++) {
        coplist[clidx] = (plane >> 16) & 0xffff;
        coplist[clidx + 2] = plane & 0xffff;
        plane += screenrow_bytes;
        clidx += 4;
    }
}

/**
 * Private function to build the main copper list.
 * For now, this is very basic and simple. Going forward, we absolutely need
 * a copper list compiler, to allow for complex sprite multiplexing and color management
 */
void ratr0_display_init_copper_list(UINT16 coplist[], int num_words,
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

    // establish the display mode
    set_display_mode(coplist, info,
                     display_info.vp_width, display_info.depth);

    // 2. Initialize the sprites with the NULL address (8x8 = 64 bytes)
    for (int i = 0; i < 8; i++) {
        ratr0_display_set_sprite(coplist, num_words, info,
                                 i, NULL_SPRITE_DATA);
    }
    set_display_surface(coplist, num_words,
                        info, &display_buffer[ratr0_front_buffer].surface);
}

static int display_buffer_size;
static void build_display_buffer(struct Ratr0DisplayInfo *init_data)
{
    display_buffer_size = init_data->buffer_width / 8 * init_data->buffer_height
        * init_data->depth;
    //PRINT_DEBUG("# BUFFERS INITIALIZED: %u", init_data->num_buffers);
    for (int i = 0; i < init_data->num_buffers; i++) {
        display_buffer[i].buffernum = i;
        display_buffer[i].surface.width = init_data->buffer_width;
        display_buffer[i].surface.height = init_data->buffer_height;

        display_buffer[i].surface.depth = init_data->depth;
        display_buffer[i].surface.is_interleaved = TRUE;
        // display buffer memory is allocated directly from the OS, otherwise the memory allocator
        // gets too inflexible
        display_buffer[i].surface.buffer = (void *) AllocMem(display_buffer_size, MEMF_CHIP|MEMF_CLEAR);
        if (display_buffer[i].surface.buffer == NULL) {
            PRINT_DEBUG("ERROR: can't allocate memory for display buffers !");
            break;
        }
    }
}

static void free_display_buffer(void)
{
    for (int i = 0; i < display_info.num_buffers; i++) {
        FreeMem(display_buffer[i].surface.buffer, display_buffer_size);
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

struct Ratr0RenderingSystem *ratr0_display_startup(Ratr0Engine *eng,
                                                   struct Ratr0DisplayInfo *init_data)
{
    engine = eng;
    rendering_system.shutdown = &ratr0_display_shutdown;
    frames_elapsed = 0;

    ratr0_sprites_startup(eng);
    ratr0_blitter_startup(eng);

    LoadView(NULL);  // clear display, reset hardware registers
    WaitTOF();       // 2 WaitTOFs to wait for 1. long frame and
    WaitTOF();       // 2. short frame copper lists to finish (if interlaced)

    /*
     * Store the display information so we can set up a default copper list.
     */
    display_info.vp_width = init_data->vp_width;
    display_info.vp_height = init_data->vp_height;
    display_info.buffer_width = init_data->buffer_width;
    display_info.buffer_height = init_data->buffer_height;
    display_info.depth = init_data->depth;
    display_info.num_buffers = init_data->num_buffers;
    display_info.is_pal = (((struct GfxBase *) GfxBase)->DisplayFlags & PAL) == PAL;

    // Build the display buffer
    build_display_buffer(&display_info);
    ratr0_display_init_copper_list(default_copper, DEFAULT_COPPER_SIZE_WORDS,
                                   &DEFAULT_COPPER_INFO);

    current_coplist = default_copper;
    current_copper_info = &DEFAULT_COPPER_INFO;
    current_coplist_size = DEFAULT_COPPER_SIZE_WORDS;

    custom.cop1lc = (ULONG) default_copper;

    _install_interrupts();

    // initialize the bitsets for dirty rectangles
    ratr0_bitset_clear(bitset_arr[ratr0_back_buffer], BITSET_SIZE);
    ratr0_bitset_clear(bitset_arr[ratr0_front_buffer], BITSET_SIZE);

    // Object management initialization
    next_hw_sprite = next_bob = 0;
    PRINT_DEBUG("Startup finished");
    return &rendering_system;
}

void ratr0_display_set_copperlist(UINT16 *copperlist, int size,
                                  struct Ratr0CopperListInfo *info)
{
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
    result->sprite_data = (UINT16 *) engine->memory_system->block_address(sheet->h_imgdata);

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
    fprintf(fp, "UINT16 coplist[] = {\n\t");
    for (int i = 0; i < len; i++) {
        if (i > 0) fprintf(fp, ", ");
        fprintf(fp, "0x%03x", copperlist[i]);
    }
    fprintf(fp, "}\n");
    fclose(fp);
}
