#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/events.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[31mEVENTS\033[0m", __VA_ARGS__)

extern void ratr0_events_startup()
{
    PRINT_DEBUG("Start up...");
    PRINT_DEBUG("Startup finished.");
}

extern void ratr0_events_shutdown()
{
    PRINT_DEBUG("Shutting down...");
    PRINT_DEBUG("Shutdown finished.");
}
