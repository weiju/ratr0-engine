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
#include <ratr0/memory.h>
#include <ratr0/amiga/hw_registers.h>
#include <ratr0/amiga/display.h>
#include <ratr0/amiga/sprites.h>
#include <ratr0/amiga/blitter.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[32mDISPLAY\033[0m", __VA_ARGS__)

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

// For our interrupt handlers
static struct Interrupt vbint, bltint, *old_bltint;
static BOOL has_blitint;
static UINT32 counter = 0, blitcounter = 0;

void VertBServer(void)
{
    // TODO: Handle vertical blank interrupts here
    //counter++;
}
void BlitHandler(void)
{
    // TODO: Handle blitter finished interrupts here, e.g.
    // Queue processing
    //blitcounter++;
    custom.intreq = INTF_BLIT;
}

static struct Ratr0AmigaDisplayInfo display_info;
static Ratr0MemHandle h_copper_list;
static UINT16 *copper_list;
static Ratr0Engine *engine;

// Data fetch
#define DDFSTRT_VALUE      0x0038
#define DDFSTOP_VALUE      0x00d0

// Display window
#define DIWSTRT_VALUE      0x2c81
#define DIWSTOP_VALUE_PAL  0x2cc1
#define DIWSTOP_VALUE_NTSC 0xf4c1


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

void ratr0_amiga_wait_vblank(void)
{
    WaitTOF();
}

/**
 * We need to adjust the BPLCONx and BPLxMOD values after changing the
 * display mode.
 * Note: bplmod is currently the same for BPL1MOD and BPL2MOD, so no
 * dual playfield as for now
 */
void set_display_mode(UINT16 width, UINT8 num_bitplanes)
{
    // width *needs* to be a multiple of 16 because Amiga playfield hardware operates
    // on word boundaries
    UINT16 screenrow_bytes = width / 8;
    UINT16 bplmod = screenrow_bytes * (num_bitplanes - 1);
    copper_list[bplcon0_idx] = (num_bitplanes << 12) | 0x200;
    copper_list[bpl1mod_idx] = bplmod;
    copper_list[bpl1mod_idx + 2] = bplmod;
}

/**
 * Set the bitplane pointers in the copper list to the specified display buffer
 * It also adjusts BPLCONx and BPLxMOD.
 */
static struct Ratr0AmigaRenderContext render_context;
Ratr0MemHandle h_display_buffer;

struct Ratr0AmigaRenderContext *ratr0_amiga_get_render_context(void)
{
    return &render_context;
}

/**
 * Private function to apply the render context to the copper list
 */
static void set_render_context(struct Ratr0AmigaRenderContext *ctx)
{
    UINT16 screenrow_bytes = ctx->width / 8;
    set_display_mode(ctx->width, ctx->depth);
    UINT32 plane = (UINT32) ctx->display_buffer;
    UINT32 clidx = bpl1pth_idx;

    for (int i = 0; i < ctx->depth; i++) {
        copper_list[clidx] = (plane >> 16) & 0xffff;
        copper_list[clidx + 2] = plane & 0xffff;
        plane += screenrow_bytes;
        clidx += 4;
    }
}

/**
 * Private function to build the main copper list.
 */
static void build_copper_list()
{
    int cl_index = 0;
    cl_index = _cop_move(FMODE, 0, cl_index);  // 4 bytes

    // Initialize the sprites with the NULL address (8x8 = 64 bytes)
    UINT16 spr_ptr = SPR0PTH;
    spr0pth_idx = cl_index + 1;
    for (int i = 0; i < 8; i++) {
        cl_index = _cop_move(spr_ptr, (((ULONG) NULL_SPRITE_DATA) >> 16) & 0xffff, cl_index);
        cl_index = _cop_move(spr_ptr + 2, ((ULONG) NULL_SPRITE_DATA) & 0xffff, cl_index);
        spr_ptr += 4;
    }

    // set up the display and DMA windows (16 bytes)
    cl_index = _cop_move(DDFSTRT, DDFSTRT_VALUE, cl_index);
    cl_index = _cop_move(DDFSTOP, DDFSTOP_VALUE, cl_index);
    cl_index = _cop_move(DIWSTRT, DIWSTRT_VALUE, cl_index);
    if (display_info.is_pal) {
        cl_index = _cop_move(DIWSTOP, DIWSTOP_VALUE_PAL, cl_index);
    } else {
        cl_index = _cop_move(DIWSTOP, DIWSTOP_VALUE_NTSC, cl_index);
    }

    // TODO: set up the bitmap for the display
    // 1. BPLCONx (8 bytes)
    bplcon0_idx = cl_index + 1;
    cl_index = _cop_move(BPLCON0, 0, cl_index); // 2 bitplanes for splash screen
    cl_index = _cop_move(BPLCON2, 0x0060, cl_index); // playfield 2 priority+sprite priority

    // 2. BPLxMOD (8 bytes)
    bpl1mod_idx = cl_index + 1;
    cl_index = _cop_move(BPL1MOD, 0, cl_index);
    cl_index = _cop_move(BPL2MOD, 0, cl_index);

    // 3. COLORxx (32 colors => 32 * 4 = 128 bytes)
    color00_idx = cl_index + 1;
    for (int i = 0; i < 32; i++) {
        cl_index = _cop_move(COLOR00 + i * 2, 0, cl_index);
    }
    // 4. BPLxPTH/BPLxPTL (NUM_BITLANES * 8 bytes => 12 bytes)
    bpl1pth_idx = cl_index + 1;
    for (int i = 0; i < MAX_BITPLANES; i++) {
        cl_index = _cop_move(BPL1PTH + i * 4, 0, cl_index);
        cl_index = _cop_move(BPL1PTL + i * 4, 0, cl_index);
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
    set_render_context(&render_context);
}

static void build_display_buffer(struct Ratr0AmigaDisplayInfo *init_data)
{
    render_context.width = init_data->width;
    render_context.height = init_data->height;
    render_context.depth = init_data->depth;
    render_context.is_interleaved = TRUE;
    // TODO: double buffer
    h_display_buffer = engine->memory_system->allocate_block(RATR0_MEM_CHIP,
                                                             init_data->width / 8 *
                                                             init_data->height *
                                                             init_data->depth);
    render_context.display_buffer = engine->memory_system->block_address(h_display_buffer);
}

// Blitter and sprite queues here
//struct Ratr0

void _install_interrupts(void)
{
    // Interrupt server for Vertical Blank, only PORTS, COPER, VERTB, EXTER and NMI
    // can be serviced throught an interrupt server because they are chained
    vbint.is_Node.ln_Type = NT_INTERRUPT;
    vbint.is_Node.ln_Pri = -60;
    vbint.is_Node.ln_Name = "vbinter";
    vbint.is_Data = (APTR) &counter;
    vbint.is_Code = VertBServer;
    AddIntServer(INTB_VERTB, &vbint);

    // Interrupt handler for BLIT, blit finished has to be serviced through
    // handler
    bltint.is_Node.ln_Type = NT_INTERRUPT;
    bltint.is_Node.ln_Pri = -60;
    bltint.is_Node.ln_Name = "vbinter";
    bltint.is_Data = (APTR) &blitcounter;
    bltint.is_Code = BlitHandler;

    // Replace blit handler, and remember old state
    has_blitint = custom.intenar & INTF_BLIT ? TRUE : FALSE;
    custom.intena = INTF_BLIT;
    old_bltint = SetIntVector(INTB_BLIT, &bltint);
    custom.intena = INTF_SETCLR | INTF_BLIT;
}

void _uninstall_interrupts(void)
{
    // remove blitter handler
    custom.intena = INTF_BLIT;
    SetIntVector(INTB_BLIT, old_bltint);
    if (has_blitint) {
        custom.intena = INTF_SETCLR | INTF_BLIT;
    }

    // Remove vertical blank handler
    RemIntServer(INTB_VERTB, &vbint);
}

void ratr0_amiga_display_startup(Ratr0Engine *eng, struct Ratr0AmigaDisplayInfo *init_data)
{
    engine = eng;
    ratr0_amiga_sprites_startup(eng);
    ratr0_amiga_blitter_startup(eng);

    LoadView(NULL);  // clear display, reset hardware registers
    WaitTOF();       // 2 WaitTOFs to wait for 1. long frame and
    WaitTOF();       // 2. short frame copper lists to finish (if interlaced)

    /*
     * Store the display information so we can set up a default copper list.
     */
    display_info.width = init_data->width;
    display_info.height = init_data->height;
    display_info.depth = init_data->depth;
    display_info.is_pal = (((struct GfxBase *) GfxBase)->DisplayFlags & PAL) == PAL;

    int cl_num_bytes = 260;
    h_copper_list = engine->memory_system->allocate_block(RATR0_MEM_CHIP, cl_num_bytes);
    copper_list = engine->memory_system->block_address(h_copper_list);

    // Build the display buffer
    build_display_buffer(init_data);
    build_copper_list();
    custom.cop1lc = (ULONG) copper_list;

    _install_interrupts();
}

void ratr0_amiga_display_shutdown(void)
{
    _uninstall_interrupts();

    PRINT_DEBUG("Copper list size: %d", copperlist_size);
    engine->memory_system->free_block(h_copper_list);
    ratr0_amiga_sprites_shutdown();

    // Restore the Workbench display by restoring the original copper list
    LoadView(((struct GfxBase *) GfxBase)->ActiView);
    WaitTOF();
    WaitTOF();
    custom.cop1lc = (ULONG) ((struct GfxBase *) GfxBase)->copinit;
    RethinkDisplay();
}

void ratr0_amiga_set_palette(UINT16 *colors, UINT8 num_colors)
{
    for (int i = 0; i < num_colors; i++) {
        copper_list[color00_idx + i * 2] = colors[i];
    }
}

void ratr0_amiga_display_set_sprite(int sprite_num, UINT16 *data)
{
    int spr_idx = spr0pth_idx + 2 * sprite_num;
    copper_list[spr_idx] = ((UINT32) data >> 16) & 0xffff;
    copper_list[spr_idx + 2] = (UINT32) data & 0xffff;
}

void ratr0_amiga_display_update()
{
    // We need to find and restore dirty rectangles. This is rather hard, because on Amiga, there
    // are multiple ways to do that. We can have a restore buffer which is the easiest, but
    // if the game is double buffered, we need to have 3 buffers
    // An alternative is using tile map based restoration.

    // We might be able to use priority queues for sorting our BOBs, so we can draw them from
    // top to bottom
    // Same for sprites, but we need to interact with the copper list for multiplexing
}
