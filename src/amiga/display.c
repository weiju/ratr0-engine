#include <stdio.h>

#include <hardware/custom.h>
#include <graphics/gfxbase.h>
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>

#include <ratr0/debug_utils.h>
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

    int num_entries = 6;
    h_copper_list = Ratr0MemoryService.allocate_block(RATR0_MEM_CHIP, num_entries * sizeof(UINT16));
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
