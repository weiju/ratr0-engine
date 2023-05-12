/** @file engine.c */
#include <stdio.h>

#include <ratr0/debug_utils.h>
#include <ratr0/engine.h>
#include <ratr0/timers.h>
#include <ratr0/memory.h>
#include <ratr0/events.h>
#include <ratr0/audio.h>
#include <ratr0/rendering.h>
#include <ratr0/input.h>
#include <ratr0/resources.h>
#include <ratr0/scenes.h>

#define MAX_TIMERS (10)
#ifdef AMIGA
#include <ratr0/amiga/engine.h>
#else
#include <ratr0/SDL/engine.h>
#endif

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mENGINE\033[0m", __VA_ARGS__)

static Ratr0Engine engine;
void ratr0_engine_shutdown(void);

Ratr0Engine *ratr0_engine_startup(void)
{
    // hook in the shutdown function
    engine.shutdown = &ratr0_engine_shutdown;
#ifdef AMIGA
    ratr0_amiga_engine_startup(&engine);
#else
    ratr0_sdl_engine_startup(&engine);
#endif

    /* Just an example for a configuration, should come from a config file */
    struct Ratr0DisplayInfo display_init = { 320, 256, 320, 256, 3, 2 };
    struct Ratr0MemoryConfig mem_config = {
        8192, 20,   // 8k general purpose memory with max 20 mem blocks
        65536, 20  // 64k chip memory with max 20 mem blocks
    };

    PRINT_DEBUG("Start up...");

    engine.memory_system = ratr0_memory_startup(&engine, &mem_config);
    engine.event_system = ratr0_events_startup(&engine);
    engine.timer_system = ratr0_timers_startup(&engine, MAX_TIMERS);
    engine.input_system = ratr0_input_startup(&engine);
    engine.rendering_system = ratr0_rendering_startup(&engine, &display_init);
    engine.audio_system = ratr0_audio_startup(&engine);
    engine.resource_system = ratr0_resources_startup(&engine);
    engine.scenes_system = ratr0_scenes_startup(&engine);
    PRINT_DEBUG("Startup finished.");
    return &engine;
}

void ratr0_engine_shutdown(void)
{
    PRINT_DEBUG("Shutting down...");
    engine.scenes_system->shutdown();
    engine.resource_system->shutdown();
    engine.audio_system->shutdown();
    engine.rendering_system->shutdown();
    engine.input_system->shutdown();
    engine.timer_system->shutdown();
    engine.event_system->shutdown();
    engine.memory_system->shutdown();
#ifdef USE_SDL2
    ratr0_sdl_engine_shutdown();
#endif /* USE_SDL2 */

    PRINT_DEBUG("Shutdown finished.");
}
