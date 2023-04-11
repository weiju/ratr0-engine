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
#include <ratr0/scenes.h>
#include <ratr0/scripting.h>

#define MAX_TIMERS (10)
#ifdef AMIGA
#include <ratr0/amiga/engine.h>
#endif

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mENGINE\033[0m", __VA_ARGS__)

static Ratr0Engine engine;
void ratr0_engine_shutdown(void);
void ratr0_engine_game_loop(void);

Ratr0Engine *ratr0_engine_startup(void)
{
    // hook in the shutdown function
    engine.shutdown = &ratr0_engine_shutdown;
#ifdef AMIGA
    ratr0_amiga_engine_startup(&engine);
    engine.game_loop = &ratr0_amiga_engine_game_loop;
#else
    engine.game_loop = &ratr0_engine_game_loop;
#endif

    /* Just an example for a configuration, should come from a config file */
    struct Ratr0DisplayInfo display_init = { 320, 200, 3 };
    struct Ratr0MemoryConfig mem_config = {
        8192, 20,  // 8k chip memory with max 20 mem blocks
        8192, 20   // 8k general purpose memory with max 20 mem blocks
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
    engine.memory_system = ratr0_memory_startup(&engine, &mem_config);
    engine.event_system = ratr0_events_startup(&engine);
    engine.timer_system = ratr0_timers_startup(&engine, MAX_TIMERS);
    engine.input_system = ratr0_input_startup(&engine);
    engine.display_system = ratr0_display_startup(&engine, &display_init);
    engine.audio_system = ratr0_audio_startup(&engine);
    engine.resource_system = ratr0_resources_startup(&engine);
    engine.physics_system = ratr0_physics_startup(&engine);
    engine.scene_system = ratr0_scenes_startup(&engine);
    engine.scripting_system = ratr0_scripting_startup(&engine);
    PRINT_DEBUG("Startup finished.");
    return &engine;
}

void ratr0_engine_shutdown(void)
{
    PRINT_DEBUG("Shutting down...");
    engine.scripting_system->shutdown();
    engine.scene_system->shutdown();
    engine.physics_system->shutdown();
    engine.resource_system->shutdown();
    engine.audio_system->shutdown();
    engine.display_system->shutdown();
    engine.input_system->shutdown();
    engine.timer_system->shutdown();
    engine.event_system->shutdown();
    engine.memory_system->shutdown();
#ifdef USE_SDL2
    // SDL shutdown
    SDL_Quit();
    PRINT_DEBUG("SDL Quit");
#endif /* USE_SDL2 */

    PRINT_DEBUG("Shutdown finished.");
}


#ifndef AMIGA
void ratr0_engine_game_loop(void)
{
    SDL_Window *window = SDL_CreateWindow("RATR0 Engine",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          680, 480,
                                          0);

    if(!window) {
        printf("Failed to create window\n");
        return;
    }
    SDL_Surface *window_surface = SDL_GetWindowSurface(window);

    if (!window_surface) {
        printf("Failed to get the surface from the window\n");
        return;
    }


    SDL_Event e;
    BOOL keep_window_open = TRUE;
    while(keep_window_open) {
        while(SDL_PollEvent(&e) > 0) {
            switch(e.type) {
            case SDL_QUIT:
                keep_window_open = FALSE;
                break;
            }
            SDL_UpdateWindowSurface(window);
        }
    }
}
#endif /* AMIGA */
