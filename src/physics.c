#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/physics.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[35;1mPHYSICS\033[0m", __VA_ARGS__)

static struct Ratr0PhysicsSystem physics_system;
static Ratr0Engine *engine;
void ratr0_physics_shutdown(void);

struct Ratr0PhysicsSystem *ratr0_physics_startup(Ratr0Engine *eng)
{
    engine = eng;
    physics_system.shutdown = &ratr0_physics_shutdown;
    PRINT_DEBUG("Startup finished.");
    return &physics_system;
}

void ratr0_physics_shutdown(void)
{
    PRINT_DEBUG("Shutdown finished.");
}
