#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/amiga/display.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[32mDISPLAY[AMIGA]\033[0m", __VA_ARGS__)

extern void ratr0_amiga_display_startup()
{
    PRINT_DEBUG("Start up...");
    PRINT_DEBUG("Startup finished.");
}

extern void ratr0_amiga_display_shutdown()
{
    PRINT_DEBUG("Shutting down...");
    PRINT_DEBUG("Shutdown finished.");
}
