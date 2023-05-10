/** @file engine.h
 *
 * SDL specific engine implementation.
 */
#pragma once
#ifndef __RATR0_SDL_ENGINE_H__
#define __RATR0_SDL_ENGINE_H__
#include <ratr0/engine.h>

/**
 * Game loop function.
 */
void ratr0_sdl_engine_game_loop(void);

/**
 * Forces engine exit.
 */
void ratr0_sdl_engine_exit(void);

/**
 * Starts up the engine.
 *
 * @param engine pointer to Ratr0Engine instance
 */
void ratr0_sdl_engine_startup(Ratr0Engine *engine);

/**
 * Shuts down the engine.
 */
void ratr0_sdl_engine_shutdown(void);

#endif /* __RATR0_SDL_ENGINE_H__ */
