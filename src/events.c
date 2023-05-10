/** @file events.c */
#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/events.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[31mEVENTS\033[0m", __VA_ARGS__)

static struct Ratr0EventSystem event_system;
void ratr0_events_shutdown(void);
static Ratr0Engine *engine;

struct Ratr0EventSystem *ratr0_events_startup(Ratr0Engine *eng)
{
    engine = eng;
    event_system.shutdown = &ratr0_events_shutdown;
    PRINT_DEBUG("Startup finished.");
    return &event_system;
}

void ratr0_events_shutdown(void)
{
    PRINT_DEBUG("Shutdown finished.");
}
