#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/display.h>

#ifdef AMIGA
#include <ratr0/amiga/display.h>
static struct Ratr0AmigaDisplayInfo display_info;
#endif /* AMIGA */

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[32mDISPLAY\033[0m", __VA_ARGS__)

void ratr0_display_startup(struct Ratr0DisplayInfo *init_data)
{
    PRINT_DEBUG("Start up...");
#ifdef AMIGA
    display_info.width = init_data->width;
    display_info.height = init_data->height;
    display_info.depth = init_data->depth;

    ratr0_amiga_display_startup(&display_info);
#endif
    PRINT_DEBUG("Startup finished.");
}

void ratr0_display_shutdown(void)
{
    PRINT_DEBUG("Shutting down...");
#ifdef AMIGA
    ratr0_amiga_display_shutdown();
#endif
    PRINT_DEBUG("Shutdown finished.");
}
