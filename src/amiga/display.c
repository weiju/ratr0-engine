#include <stdio.h>

#include <hardware/custom.h>
#include <graphics/gfxbase.h>
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>

#include <ratr0/debug_utils.h>
#include <ratr0/amiga/display.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[32mDISPLAY[AMIGA]\033[0m", __VA_ARGS__)

/*
 * We assume these to exist.
 */
extern struct GfxBase *GfxBase;
extern struct Custom custom;

void ratr0_amiga_display_startup(void)
{
    PRINT_DEBUG("Start up...");
    LoadView(NULL);  // clear display, reset hardware registers
    WaitTOF();       // 2 WaitTOFs to wait for 1. long frame and
    WaitTOF();       // 2. short frame copper lists to finish (if interlaced)
    //return (((struct GfxBase *) GfxBase)->DisplayFlags & PAL) == PAL;
    PRINT_DEBUG("Startup finished.");
}

void ratr0_amiga_display_shutdown(void)
{
    PRINT_DEBUG("Shutting down...");
    // Restore the Workbench display by restoring the original copper list
    LoadView(((struct GfxBase *) GfxBase)->ActiView);
    WaitTOF();
    WaitTOF();
    custom.cop1lc = (ULONG) ((struct GfxBase *) GfxBase)->copinit;
    RethinkDisplay();
    PRINT_DEBUG("Shutdown finished.");
}
