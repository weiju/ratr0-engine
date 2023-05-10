/** @file rendering.c */
#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/rendering.h>

#ifdef AMIGA
#include <ratr0/amiga/display.h>
#endif /* AMIGA */

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[32mDISPLAY\033[0m", __VA_ARGS__)

static struct Ratr0RenderingSystem rendering_system;
void ratr0_rendering_shutdown(void);

struct Ratr0RenderingSystem *ratr0_rendering_startup(Ratr0Engine *eng,
                                                     struct Ratr0DisplayInfo *init_data)
{
    rendering_system.shutdown = &ratr0_rendering_shutdown;

#ifdef AMIGA
    ratr0_amiga_display_startup(eng, &rendering_system, init_data);
#endif
    PRINT_DEBUG("Startup finished.");
    return &rendering_system;
}

void ratr0_rendering_shutdown(void)
{
#ifdef AMIGA
    ratr0_amiga_display_shutdown();
#endif
    PRINT_DEBUG("Shutdown finished.");
}
