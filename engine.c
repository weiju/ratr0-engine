#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/engine.h>
#include <ratr0/timers.h>

extern void ratr0_engine_startup()
{
    PRINT_DEBUG("ENGINE - Start up...\n");
    ratr0_timers_startup();
    PRINT_DEBUG("ENGINE - Startup done.\n");
}

extern void ratr0_engine_shutdown()
{
    PRINT_DEBUG("ENGINE - Shutting down...\n");
    ratr0_timers_shutdown();
    PRINT_DEBUG("ENGINE - Shutdown done.\n");
}
