#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/display.h>

#ifdef AMIGA
#include <ratr0/amiga/display.h>
#endif /* AMIGA */

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[32mDISPLAY\033[0m", __VA_ARGS__)

void ratr0_display_startup(void)
{
    PRINT_DEBUG("Start up...");
    PRINT_DEBUG("Startup finished.");
}

void ratr0_display_shutdown(void)
{
    PRINT_DEBUG("Shutting down...");
    PRINT_DEBUG("Shutdown finished.");
}
