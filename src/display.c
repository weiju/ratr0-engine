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

// Double buffer management
#define MAX_BUFFERS (2)
static void set_display_surface(struct Ratr0Surface *s);
static struct Ratr0Surface display_surface[MAX_BUFFERS];
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
struct Ratr0AnimatedHWSprite hw_sprite_table[20];
UINT16 next_hw_sprite = 0;
struct Ratr0AnimatedBob bob_table[20];
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
    set_display_surface(&display_surface[ratr0_front_buffer]);
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

static struct Ratr0DisplayInfo display_info;
static Ratr0MemHandle h_copper_list;
static UINT16 *copper_list;

// DDFSTRT (lores) = DIWSTRT_h / 2 - 8.5
// so $81 -> $38
// DDFSTRT = DDFSTOP-(8*(word count-1))for low resolution
// DDFSTOP = DDFSTRT + (8 * (word count - 1))
// Data fetch for a 320 pixel wide image: DDFSTRT = 0x38, DDFSTOP = 0xd0
// Data fetch for a 288 pixel wide image: DDFSTRT = 0x46, DDFSTOP = 0xce
#define DDFSTRT_VALUE_320      0x0038
#define DDFSTOP_VALUE_320      0x00d0

// 1. 40/c8
#define DDFSTRT_VALUE_288      0x0040
#define DDFSTOP_VALUE_288      0x00c8

// Display window
#define DIWSTRT_VALUE_320      0x2c81
#define DIWSTOP_VALUE_PAL_320  0x2cc1
#define DIWSTOP_VALUE_NTSC_320 0xf4c1

// DDFSTRT = DIWSTRT / 2 - 8.5
// DIWSTRT = (DDFSTRT + 8.5) * 2
// DIWSTRT_h = (DDFSTRT_h + 8.5) * 2
// 288 horizontal centered
#define DIWSTRT_VALUE_288      0x2c91
#define DIWSTOP_VALUE_PAL_288  0x2cb1
#define DIWSTOP_VALUE_NTSC_288 0xf4b1


/* This is a bit of a trick: I pre-allocate dummy sprite data */
UINT16 __chip NULL_SPRITE_DATA[] = {
    0x0000, 0x0000,
    0x0000, 0x0000
};

UINT16 _cop_move(UINT16 addr, UINT16 value, UINT16 index)
{
    copper_list[index++] = addr;
    copper_list[index++] = value;
    return index;
}
UINT16 _cop_wait_end(UINT16 index)
{
    copper_list[index++] = 0xffff;
    copper_list[index++] = 0xfffe;
    return index;
}

#define MAX_BITPLANES (6)
static int copperlist_size = 0;

// The principal copper list indexes to build the display elements
static int bpl1pth_idx;
static int color00_idx;
static int spr0pth_idx;
static int bplcon0_idx;
static int bpl1mod_idx;

/**
 * We need to adjust the BPLCONx and BPLxMOD values after changing the
 * display mode.
 * Note: bplmod is currently the same for BPL1MOD and BPL2MOD, so no
 * dual playfield as for now
 */
static void set_display_mode(UINT16 width, UINT8 num_bitplanes)
{
    // width *needs* to be a multiple of 16 because Amiga playfield hardware operates
    // on word boundaries
    UINT16 screenrow_bytes = width / 8;
    UINT16 bplmod = screenrow_bytes * (num_bitplanes - 1);
    UINT16 bplcon0_value = (num_bitplanes << 12) | 0x200;
    PRINT_DEBUG("screenrow_bytes: %d bpl1mod: %d num_bitplanes: %d bplcon0: %04x",
                (int) screenrow_bytes, (int) bplmod, (int) num_bitplanes, (int) bplcon0_value);
    copper_list[bplcon0_idx] = bplcon0_value;
    copper_list[bpl1mod_idx] = bplmod;
    copper_list[bpl1mod_idx + 2] = bplmod;
}

/**
 * Set the bitplane pointers in the copper list to the specified display buffer
 * It also adjusts BPLCONx and BPLxMOD.
 */
struct Ratr0Surface *ratr0_get_front_buffer(void)
{
    return &display_surface[ratr0_front_buffer];
}
struct Ratr0Surface *ratr0_get_back_buffer(void)
{
    return &display_surface[ratr0_back_buffer];
}

/**
 * Private function to apply the render context to the copper list
 */
static void set_display_surface(struct Ratr0Surface *s)
{
    UINT16 screenrow_bytes = s->width / 8;
    UINT32 plane = (UINT32) s->buffer;
    UINT32 clidx = bpl1pth_idx;

    for (int i = 0; i < s->depth; i++) {
        copper_list[clidx] = (plane >> 16) & 0xffff;
        copper_list[clidx + 2] = plane & 0xffff;
        plane += screenrow_bytes;
        clidx += 4;
    }
}

/**
 * Private function to build the main copper list.
 * For now, this is very basic and simple. Going forward, we absolutely need
 * a copper list compiler, to allow for complex sprite multiplexing and color management
 */
static void build_copper_list(struct Ratr0DisplayInfo *init_data)
{
    int cl_index = 0;
    cl_index = _cop_move(FMODE, 0, cl_index);  // 4 bytes

    // set up the display and DMA windows (16 bytes)
    // TODO:  look at the viewport size to build display window and DMA start
    if (init_data->vp_width == 288) {
        PRINT_DEBUG("288 DDFSTRT");
        cl_index = _cop_move(DDFSTRT, DDFSTRT_VALUE_288, cl_index);
        cl_index = _cop_move(DDFSTOP, DDFSTOP_VALUE_288, cl_index);
        cl_index = _cop_move(DIWSTRT, DIWSTRT_VALUE_288, cl_index);
        if (display_info.is_pal) {
            cl_index = _cop_move(DIWSTOP, DIWSTOP_VALUE_PAL_288, cl_index);
        } else {
            cl_index = _cop_move(DIWSTOP, DIWSTOP_VALUE_NTSC_288, cl_index);
        }
    } else {
        PRINT_DEBUG("320 DDFSTRT");
        cl_index = _cop_move(DDFSTRT, DDFSTRT_VALUE_320, cl_index);
        cl_index = _cop_move(DDFSTOP, DDFSTOP_VALUE_320, cl_index);
        if (display_info.is_pal) {
            cl_index = _cop_move(DIWSTOP, DIWSTOP_VALUE_PAL_320, cl_index);
        } else {
            cl_index = _cop_move(DIWSTOP, DIWSTOP_VALUE_NTSC_320, cl_index);
        }
    }

    // 1. BPLCONx (8 bytes)
    bplcon0_idx = cl_index + 1;
    cl_index = _cop_move(BPLCON0, 0, cl_index); // 2 bitplanes for splash screen
    cl_index = _cop_move(BPLCON2, 0x0060, cl_index); // playfield 2 priority+sprite priority

    // 2. BPLxMOD (8 bytes)
    bpl1mod_idx = cl_index + 1;
    cl_index = _cop_move(BPL1MOD, 0, cl_index);
    cl_index = _cop_move(BPL2MOD, 0, cl_index);

    // 3. BPLxPTH/BPLxPTL (NUM_BITLANES * 8 bytes => 12 bytes)
    bpl1pth_idx = cl_index + 1;
    for (int i = 0; i < MAX_BITPLANES; i++) {
        cl_index = _cop_move(BPL1PTH + i * 4, 0, cl_index);
        cl_index = _cop_move(BPL1PTL + i * 4, 0, cl_index);
    }

    // 4. Initialize the sprites with the NULL address (8x8 = 64 bytes)
    UINT16 spr_ptr = SPR0PTH;
    spr0pth_idx = cl_index + 1;
    for (int i = 0; i < 8; i++) {
        cl_index = _cop_move(spr_ptr, (((ULONG) NULL_SPRITE_DATA) >> 16) & 0xffff, cl_index);
        cl_index = _cop_move(spr_ptr + 2, ((ULONG) NULL_SPRITE_DATA) & 0xffff, cl_index);
        spr_ptr += 4;
    }

    // 5. COLORxx (32 colors => 32 * 4 = 128 bytes)
    color00_idx = cl_index + 1;
    for (int i = 0; i < 32; i++) {
        cl_index = _cop_move(COLOR00 + i * 2, 0, cl_index);
    }

    // End the copper list (4 bytes)
    cl_index = _cop_wait_end(cl_index);

    // Set copperlist values please be aware that the values are
    // at the odd indexes
    copper_list[color00_idx] = 0x0fff;
    copper_list[color00_idx + 2] = 0x0ccc;
    copper_list[color00_idx + 4] = 0x0888;
    copper_list[color00_idx + 6] = 0x0000;

    // Sprite colors start at color17
    // just for debugging
    copper_list[color00_idx + 17 * 2] = 0x000f;
    copper_list[color00_idx + 18 * 2] = 0x0ff0;
    copper_list[color00_idx + 19 * 2] = 0x00f0;

    // Just for diagnostics
    copperlist_size = cl_index;

    set_display_surface(&display_surface[ratr0_front_buffer]);
}

static int display_buffer_size;
static void build_display_buffer(struct Ratr0DisplayInfo *init_data)
{
    display_buffer_size = init_data->buffer_width / 8 * init_data->buffer_height
        * init_data->depth;
    PRINT_DEBUG("# BUFFERS INITIALIZED: %u", init_data->num_buffers);
    for (int i = 0; i < init_data->num_buffers; i++) {
        display_surface[i].width = init_data->buffer_width;
        display_surface[i].height = init_data->buffer_height;

        display_surface[i].depth = init_data->depth;
        display_surface[i].is_interleaved = TRUE;
        // display buffer memory is allocated directly from the OS, otherwise the memory allocator
        // gets too inflexible
        display_surface[i].buffer = (void *) AllocMem(display_buffer_size, MEMF_CHIP|MEMF_CLEAR);
        if (display_surface[i].buffer == NULL) {
            PRINT_DEBUG("ERROR: can't allocate memory for display buffers !");
            break;
        }
    }
}

static void free_display_buffer(void)
{
    for (int i = 0; i < display_info.num_buffers; i++) {
        FreeMem(display_surface[i].buffer, display_buffer_size);
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

#define COPPERLIST_SIZE_BYTES (260)

struct Ratr0RenderingSystem *ratr0_display_startup(Ratr0Engine *eng,
                                                   struct Ratr0DisplayInfo *init_data)
{
    PRINT_DEBUG("DISPLAY_STARTUP");
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

    h_copper_list = engine->memory_system->allocate_block(RATR0_MEM_CHIP, COPPERLIST_SIZE_BYTES);
    copper_list = engine->memory_system->block_address(h_copper_list);

    // Build the display buffer
    build_display_buffer(&display_info);
    build_copper_list(&display_info);
    set_display_mode(init_data->vp_width, init_data->depth);
    custom.cop1lc = (ULONG) copper_list;

    _install_interrupts();

    // initialize the bitsets for dirty rectangles
    ratr0_bitset_clear(bitset_arr[ratr0_back_buffer], BITSET_SIZE);
    ratr0_bitset_clear(bitset_arr[ratr0_front_buffer], BITSET_SIZE);

    // Object management initialization
    next_hw_sprite = next_bob = 0;
    return &rendering_system;
}

void ratr0_display_shutdown(void)
{
    free_display_buffer();
    _uninstall_interrupts();

    engine->memory_system->free_block(h_copper_list);
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
        copper_list[color00_idx + (i + offset) * 2] = colors[i];
    }
}

void ratr0_display_set_sprite(int sprite_num, UINT16 *data)
{
    int spr_idx = spr0pth_idx + 2 * sprite_num;
    copper_list[spr_idx] = ((UINT32) data >> 16) & 0xffff;
    copper_list[spr_idx + 2] = (UINT32) data & 0xffff;
}

// OBJECT MANAGEMENT
struct Ratr0AnimatedHWSprite *ratr0_create_sprite(struct Ratr0TileSheet *tilesheet,
                                                  UINT8 frames[], UINT8 num_frames, UINT8 speed)
{
    // Note we can only work within the Amiga hardware sprite limitations
    // 1. Reserve memory from engine
    // 2. Convert into sprite data structure and store into allocated memory
    // 3. Return the initialized object
    UINT16 *sprite_data = ratr0_make_sprite_data(tilesheet, frames, num_frames);
    struct Ratr0AnimatedHWSprite *result = &hw_sprite_table[next_hw_sprite++];
    result->sprite_data = sprite_data;
    // store sprite information
    return result;
}

struct Ratr0AnimatedBob *ratr0_create_bob(struct Ratr0TileSheet *tilesheet,
                                          UINT8 frames[], UINT8 num_frames,
                                          UINT8 speed)
{
    if (num_frames > RATR0_MAX_ANIM_FRAMES) {
        PRINT_DEBUG("Can't create BOB with more than %d animation frames !", RATR0_MAX_ANIM_FRAMES);
        return NULL;
    }
    struct Ratr0AnimatedBob *result = &bob_table[next_bob++];
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
