#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/engine.h>
#include <ratr0/timers.h>
#include <ratr0/memory.h>
#include <ratr0/events.h>
#include <ratr0/audio.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mENGINE\033[0m", __VA_ARGS__)

extern void ratr0_engine_startup()
{
    PRINT_DEBUG("Start up...");
    ratr0_memory_startup();
    ratr0_events_startup();
    ratr0_timers_startup();
    ratr0_audio_startup();
    PRINT_DEBUG("Startup finished.");
}

extern void ratr0_engine_shutdown()
{
    PRINT_DEBUG("Shutting down...");
    ratr0_audio_shutdown();
    ratr0_timers_shutdown();
    ratr0_events_shutdown();
    ratr0_memory_shutdown();
    PRINT_DEBUG("Shutdown finished.");
}
