#include <stdio.h>

#ifdef USE_SDL2
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif /* USE_SDL2 */

#include <ratr0/debug_utils.h>
#include <ratr0/engine.h>
#include <ratr0/timers.h>
#include <ratr0/memory.h>
#include <ratr0/events.h>
#include <ratr0/audio.h>
#include <ratr0/rendering.h>
#include <ratr0/input.h>
#include <ratr0/physics.h>
#include <ratr0/resources.h>
#include <ratr0/world.h>

#define MAX_TIMERS (10)
#ifdef AMIGA
#include <ratr0/amiga/engine.h>
static volatile UWORD *custom_color00 = (volatile UWORD *) 0xdff180;
#endif
#ifdef SDL2
#endif

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mENGINE\033[0m", __VA_ARGS__)

enum { GAMESTATE_QUIT, GAMESTATE_RUNNING };

static Ratr0Engine engine;
static int game_state = GAMESTATE_RUNNING;
void ratr0_engine_shutdown(void);
void ratr0_engine_game_loop(void);
void ratr0_engine_exit(void)
{
    game_state = GAMESTATE_QUIT;
}

Ratr0Engine *ratr0_engine_startup(void)
{
    // hook in the shutdown function
    engine.shutdown = &ratr0_engine_shutdown;
    engine.exit = &ratr0_engine_exit;
#ifdef AMIGA
    ratr0_amiga_engine_startup(&engine);
#endif
    engine.game_loop = &ratr0_engine_game_loop;

    /* Just an example for a configuration, should come from a config file */
    struct Ratr0DisplayInfo display_init = { 320, 256, 3, FALSE };
    struct Ratr0MemoryConfig mem_config = {
        8192, 20,   // 8k general purpose memory with max 20 mem blocks
        65536, 20  // 128k chip memory with max 20 mem blocks
    };

    PRINT_DEBUG("Start up...");

#ifdef USE_SDL2
    // Use SDL
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) == -1) {
        PRINT_DEBUG("ERROR: Could not initialize SDL: %s.", SDL_GetError());
        exit(-1);
    }
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    PRINT_DEBUG("SDL initialized.");
#endif /* USE_SDL2 */
    engine.memory_system = ratr0_memory_startup(&engine, &mem_config);
    engine.event_system = ratr0_events_startup(&engine);
    engine.timer_system = ratr0_timers_startup(&engine, MAX_TIMERS);
    engine.input_system = ratr0_input_startup(&engine);
    engine.rendering_system = ratr0_rendering_startup(&engine, &display_init);
    engine.audio_system = ratr0_audio_startup(&engine);
    engine.resource_system = ratr0_resources_startup(&engine);
    engine.physics_system = ratr0_physics_startup(&engine);
    engine.world_system = ratr0_world_startup(&engine);
    PRINT_DEBUG("Startup finished.");
    return &engine;
}

void ratr0_engine_shutdown(void)
{
    PRINT_DEBUG("Shutting down...");
    engine.world_system->shutdown();
    engine.physics_system->shutdown();
    engine.resource_system->shutdown();
    engine.audio_system->shutdown();
    engine.rendering_system->shutdown();
    engine.input_system->shutdown();
    engine.timer_system->shutdown();
    engine.event_system->shutdown();
    engine.memory_system->shutdown();
#ifdef USE_SDL2
    // SDL shutdown
    IMG_Quit();
    SDL_Quit();
    PRINT_DEBUG("SDL Quit");
#endif /* USE_SDL2 */

    PRINT_DEBUG("Shutdown finished.");
}


void ratr0_engine_game_loop(void)
{
#ifdef AMIGA
    while (game_state != GAMESTATE_QUIT) {
        // comment in for visual timing the loop iteration
        //*custom_color00 = 0x000;
        // update all subsystems where it makes sense. Obviously it doesn't for
        // memory or resources
        engine.timer_system->update();
        engine.input_system->update();
        engine.physics_system->update();
        engine.world_system->update();
        engine.rendering_system->update();

        // For now, end when the mouse was clicked. This is just for testing
        UINT32 joystate = engine.input_system->get_joystick_state(0);
        if (joystate != 0) {
            if (joystate & JOY_FIRE0 == JOY_FIRE0) ratr0_engine_exit();
        }
        // comment in for visual timing the loop iteration
        //*custom_color00 = 0xf00;
        engine.rendering_system->wait_vblank();
    }
#else
    SDL_Window *window = SDL_CreateWindow("RATR0 Engine",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          320, 256,
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
    SDL_Surface *splash_screen = IMG_Load("ratr0_background_2planes.png");
    SDL_BlitSurface(splash_screen, NULL, window_surface, NULL);

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
    SDL_FreeSurface(splash_screen);
#endif /* AMIGA */
}
