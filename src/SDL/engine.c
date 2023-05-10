/** @file engine.c */
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <ratr0/data_types.h>
#include <ratr0/debug_utils.h>
#include <ratr0/SDL/engine.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mENGINE\033[0m", __VA_ARGS__)

void ratr0_sdl_engine_game_loop(void)
{
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
    SDL_Surface *splash_screen = IMG_Load("test_assets/ratr0_background_2planes.png");
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
}

void ratr0_sdl_engine_exit(void)
{
}

void ratr0_sdl_engine_startup(Ratr0Engine *engine)
{
    engine->game_loop = &ratr0_sdl_engine_game_loop;
    engine->exit = &ratr0_sdl_engine_exit;

    // Use SDL
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) == -1) {
        PRINT_DEBUG("ERROR: Could not initialize SDL: %s.", SDL_GetError());
        exit(-1);
    }
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    PRINT_DEBUG("SDL initialized.");
}

void ratr0_sdl_engine_shutdown(void)
{
    // SDL shutdown
    IMG_Quit();
    SDL_Quit();
    PRINT_DEBUG("SDL Quit");
}
