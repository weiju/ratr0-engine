#include <stdio.h>

#ifdef USE_SDL2
#include <SDL2/SDL.h>
#endif /* USE_SDL2 */

#include <ratr0/debug_utils.h>
#include <ratr0/engine.h>
#include <ratr0/timers.h>
#include <ratr0/memory.h>
#include <ratr0/events.h>
#include <ratr0/audio.h>
#include <ratr0/display.h>
#include <ratr0/input.h>
#include <ratr0/physics.h>
#include <ratr0/resources.h>
#include <ratr0/scripting.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mENGINE\033[0m", __VA_ARGS__)

static Ratr0Engine engine;
void ratr0_engine_shutdown(void);

Ratr0Engine *ratr0_engine_startup(void)
{
    // hook in the shutdown function
    engine.shutdown = &ratr0_engine_shutdown;

    /* Just an example for a configuration, should come from a config file */
    struct Ratr0DisplayInfo display_init = { 320, 200, 3 };
    struct Ratr0MemoryConfig mem_config = {
        4096, 10,
        4096, 10
    };

    PRINT_DEBUG("Start up...");

#ifdef USE_SDL2
    // Use SDL
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) == -1) {
        PRINT_DEBUG("ERROR: Could not initialize SDL: %s.", SDL_GetError());
        exit(-1);
    }
    PRINT_DEBUG("SDL initialized.");
#endif /* USE_SDL2 */

    engine.memory_system = ratr0_memory_startup(&mem_config);
    ratr0_events_startup();
    ratr0_timers_startup();
    ratr0_audio_startup();

    engine.display_system = ratr0_display_startup(&engine, &display_init);

    ratr0_input_startup();
    ratr0_physics_startup();
    ratr0_resources_startup();
    ratr0_scripting_startup();
    PRINT_DEBUG("Startup finished.");
    return &engine;
}

void ratr0_engine_shutdown(void)
{
    PRINT_DEBUG("Shutting down...");
    ratr0_scripting_shutdown();
    ratr0_resources_shutdown();
    ratr0_physics_shutdown();
    ratr0_input_shutdown();
    engine.display_system->shutdown();
    ratr0_audio_shutdown();
    ratr0_timers_shutdown();
    ratr0_events_shutdown();
    engine.memory_system->shutdown();

#ifdef USE_SDL2
    // SDL shutdown
    SDL_Quit();
#endif /* USE_SDL2 */

    PRINT_DEBUG("Shutdown finished.");
}
