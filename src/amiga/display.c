#include <stdio.h>

#include <clib/graphics_protos.h>

#include <ratr0/debug_utils.h>
#include <ratr0/amiga/display.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[32mDISPLAY[AMIGA]\033[0m", __VA_ARGS__)

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
    PRINT_DEBUG("Shutdown finished.");
}
