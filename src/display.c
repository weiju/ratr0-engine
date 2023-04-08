#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/display.h>

#ifdef AMIGA
#include <ratr0/amiga/display.h>
static struct Ratr0AmigaDisplayInfo display_info;
#endif /* AMIGA */

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[32mDISPLAY\033[0m", __VA_ARGS__)

static struct Ratr0DisplaySystem display_system;
void ratr0_display_shutdown(void);

struct Ratr0DisplaySystem *ratr0_display_startup(Ratr0Engine *eng, struct Ratr0DisplayInfo *init_data)
{
    display_system.shutdown = &ratr0_display_shutdown;

#ifdef AMIGA
    display_info.width = init_data->width;
    display_info.height = init_data->height;
    display_info.depth = init_data->depth;

    ratr0_amiga_display_startup(eng, &display_info);
#endif
    PRINT_DEBUG("Startup finished.");
    return &display_system;
}

void ratr0_display_shutdown(void)
{
#ifdef AMIGA
    ratr0_amiga_display_shutdown();
#endif
    PRINT_DEBUG("Shutdown finished.");
}
