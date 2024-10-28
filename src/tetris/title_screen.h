/** @file main_stage.h
 *
 * Tetra Zone title screen.
 * The title screen is the hub to the game. It displays an
 * image, waits for inut and either goes to the high score
 * screen or the main stage
 */
#pragma once
#ifndef __TITLE_SCREEN_H__
#define __TITLE_SCREEN_H__

#include <ratr0/scenes.h>
#include <ratr0/engine.h>

/**
 * Sets up the title screen scene object.
 *
 * @param engine pointer to Ratr0Engine instance
 * @return pointer to scene instance
 */
struct Ratr0Scene *setup_titlescreen_scene(Ratr0Engine *engine);

#endif /* __MAIN_SCENE_H__ */
