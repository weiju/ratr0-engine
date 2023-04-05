#include <stdio.h>

#include <hardware/custom.h>
#include <graphics/gfxbase.h>
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>

#include <ratr0/debug_utils.h>
#include <ratr0/amiga/hw_registers.h>
#include <ratr0/amiga/display.h>
#include <ratr0/memory.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[32mDISPLAY[AMIGA]\033[0m", __VA_ARGS__)

/*
  Copper instructions as reference

#define COP_MOVE(addr, data) addr, data

     FIRST WAIT INSTRUCTION WORD (IR1)
     ---------------------------------
     Bit 0           Always set to 1.
     Bits 15 - 8      Vertical beam position  (called VP).
     Bits 7 - 1       Horizontal beam position  (called HP).

     SECOND WAIT INSTRUCTION WORD (IR2)
     ----------------------------------
     Bit 0           Always set to 0.
     Bit 15          The  blitter-finished-disable bit .  Normally, this
                     bit is a 1. (See the "Advanced Topics" section below.)
     Bits 14 - 8     Vertical position compare enable bits (called VE).
     Bits 7 - 1      Horizontal position compare enable bits (called HE).

     SKIP is like WAIT, except Bit 0 of both words is 1
     Special case to finish a copper list:
       WAIT_END =>  0xffff, 0xfffe
 */

/*
 * We assume these to exist.
 */
extern struct GfxBase *GfxBase;
extern struct Custom custom;

static struct Ratr0AmigaDisplayInfo display_info;
static Ratr0MemHandle h_copper_list;
static UINT16 *copper_list;

// Data fetch
#define DDFSTRT_VALUE      0x0038
#define DDFSTOP_VALUE      0x00d0

// Display window
#define DIWSTRT_VALUE      0x2c81
#define DIWSTOP_VALUE_PAL  0x2cc1
#define DIWSTOP_VALUE_NTSC 0xf4c1


/* This is a bit of a trick: I pre-allocate dummy sprite data */
UWORD __chip NULL_SPRITE_DATA[] = {
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

void build_copper_list()
{
    int cl_index = 0;
    cl_index = _cop_move(FMODE, 0, cl_index);  // 4 bytes

    // Initialize the sprites with the NULL address (8x8 = 64 bytes)
    UINT16 spr_ptr = SPR0PTH;
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
    // 1. BPLCONx
    // 2. BPLxMOD
    // 3. COLORxx
    // 4. BPLxPTH/BPLxPTL
    // TODO: save the indexes for quick access so we can change them in the game

    // End the copper list (4 bytes)
    cl_index = _cop_wait_end(cl_index);
}
void ratr0_amiga_display_startup(struct Ratr0AmigaDisplayInfo *init_data)
{
    PRINT_DEBUG("Start up...");
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

    int cl_num_bytes = 96;
    h_copper_list = Ratr0MemoryService.allocate_block(RATR0_MEM_CHIP, cl_num_bytes);
    copper_list = Ratr0MemoryService.block_address(h_copper_list);
    build_copper_list();
    custom.cop1lc = (ULONG) copper_list;
    PRINT_DEBUG("Startup finished.");
}

void ratr0_amiga_display_shutdown(void)
{
    PRINT_DEBUG("Shutting down...");
    Ratr0MemoryService.free_block(h_copper_list);
    // Restore the Workbench display by restoring the original copper list
    LoadView(((struct GfxBase *) GfxBase)->ActiView);
    WaitTOF();
    WaitTOF();
    custom.cop1lc = (ULONG) ((struct GfxBase *) GfxBase)->copinit;
    RethinkDisplay();
    PRINT_DEBUG("Shutdown finished.");
}
