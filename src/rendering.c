#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/rendering.h>

#ifdef AMIGA
#include <ratr0/amiga/display.h>
static struct Ratr0AmigaDisplayInfo display_info;
#else
void ratr0_rendering_update(void);
void ratr0_rendering_wait_vblank(void);
#endif /* AMIGA */

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[32mDISPLAY\033[0m", __VA_ARGS__)

static struct Ratr0RenderingSystem rendering_system;
void ratr0_rendering_shutdown(void);

struct Ratr0RenderingSystem *ratr0_rendering_startup(Ratr0Engine *eng,
                                                     struct Ratr0DisplayInfo *init_data)
{
    rendering_system.shutdown = &ratr0_rendering_shutdown;

#ifdef AMIGA
    rendering_system.wait_vblank = &ratr0_amiga_wait_vblank;
    rendering_system.update = &ratr0_amiga_display_update;

    display_info.width = init_data->width;
    display_info.height = init_data->height;
    display_info.depth = init_data->depth;
    display_info.use_doublebuffer = init_data->use_doublebuffer;

    ratr0_amiga_display_startup(eng, &display_info);
#else
    rendering_system.wait_vblank = ratr0_rendering_wait_vblank;
    rendering_system.update = ratr0_rendering_update;
#endif
    PRINT_DEBUG("Startup finished.");
    return &rendering_system;
}

#ifndef AMIGA
void ratr0_rendering_update(void) { }
void ratr0_rendering_wait_vblank(void) { }
#endif

void ratr0_rendering_shutdown(void)
{
#ifdef AMIGA
    ratr0_amiga_display_shutdown();
#endif
    PRINT_DEBUG("Shutdown finished.");
}
