#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/engine.h>
#include <ratr0/timers.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mENGINE\033[0m", __VA_ARGS__)

extern void ratr0_engine_startup()
{
    PRINT_DEBUG("Start up...\n");
    ratr0_timers_startup();
    PRINT_DEBUG("Startup finished.\n");
}

extern void ratr0_engine_shutdown()
{
    PRINT_DEBUG("Shutting down...\n");
    ratr0_timers_shutdown();
    PRINT_DEBUG("Shutdown finished.\n");
}
