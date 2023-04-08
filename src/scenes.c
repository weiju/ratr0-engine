#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/scenes.h>

#ifdef AMIGA
#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[33;1mSCENES\033[0m", __VA_ARGS__)
#else
#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[34mSCENES\033[0m", __VA_ARGS__)
#endif

static struct Ratr0SceneSystem scene_system;
void ratr0_scenes_shutdown(void);
static Ratr0Engine *engine;

struct Ratr0SceneSystem *ratr0_scenes_startup(Ratr0Engine *eng)
{
    engine = eng;
    scene_system.shutdown = &ratr0_scenes_shutdown;
    PRINT_DEBUG("Startup finished.");
    return &scene_system;
}

void ratr0_scenes_shutdown(void)
{
    PRINT_DEBUG("Shutdown finished.");
}
