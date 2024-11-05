/** @file hiscore_screen.h
 *
 * Tetra Zone title screen.
 * The title screen is the hub to the game. It displays an
 * image, waits for inut and either goes to the high score
 * screen or the main stage
 */
#pragma once
#ifndef __HISCORE_SCREEN_H__
#define __HISCORE_SCREEN_H__

#include <ratr0/stages.h>
#include <ratr0/engine.h>

/**
 * Sets up the hiscore screen stage object.
 *
 * @param engine pointer to Ratr0Engine instance
 * @return pointer to stage instance
 */
struct Ratr0Stage *setup_hiscorescreen_stage(Ratr0Engine *engine);

#endif // __HISCORE_SCREEN_H__
