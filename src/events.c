#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/events.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[31mEVENTS\033[0m", __VA_ARGS__)

void ratr0_events_startup(void)
{
    PRINT_DEBUG("Startup finished.");
}

void ratr0_events_shutdown(void)
{
    PRINT_DEBUG("Shutdown finished.");
}
