/** @file engine.h
 *
 * Amiga specific engine functionality.
 */
#pragma once
#ifndef __RATR0_AMIGA_ENGINE_H__
#define __RATR0_AMIGA_ENGINE_H__

/**
 * Starts up the engine.
 *
 * @param engine pointer to Ratr0Engine instance
 */
extern void ratr0_amiga_engine_startup(Ratr0Engine *engine);

/**
 * Runs the engine game loop.
 */
extern void ratr0_amiga_engine_game_loop(void);

/**
 * Force engine exit.
 */
extern void ratr0_amiga_engine_exit(void);

#endif /* __RATR0_AMIGA_ENGINE_H__ */
