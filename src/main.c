#include <stdio.h>

#ifndef AMIGA
#include <SDL2/SDL.h>
#endif

#include <ratr0/timers.h>
#include <ratr0/engine.h>
#include <ratr0/memory.h>

void notify_timeout(void)
{
    printf("timer_timeout !!!\n");
}

#ifdef AMIGA
volatile UBYTE *ciaa_pra = (volatile UBYTE *) 0xbfe001;
#define  PRA_FIR0_BIT (1 << 6)
void waitmouse(void)
{
  while ((*ciaa_pra & PRA_FIR0_BIT) != 0) ;
}
#endif

int main(int argc, char **argv)
{
    Ratr0Timer timer;
    Ratr0Engine *engine = ratr0_engine_startup();

#ifdef AMIGA
    waitmouse();
#else
    SDL_Window *window = SDL_CreateWindow("RATR0 Engine",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          680, 480,
                                          0);

    if(!window) {
        printf("Failed to create window\n");
        return -1;
    }
    SDL_Surface *window_surface = SDL_GetWindowSurface(window);

    if (!window_surface) {
        printf("Failed to get the surface from the window\n");
        return -1;
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
#endif /* AMIGA */
    engine->shutdown();

    return 0;
}
