#pragma once
#ifndef __RATR0_SDL_ENGINE_H__
#define __RATR0_SDL_ENGINE_H__
#include <ratr0/engine.h>

void ratr0_sdl_engine_game_loop(void);
void ratr0_sdl_engine_exit(void);
void ratr0_sdl_engine_startup(Ratr0Engine *eng);
void ratr0_sdl_engine_shutdown(void);

#endif /* __RATR0_SDL_ENGINE_H__ */
