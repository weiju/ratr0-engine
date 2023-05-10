/** @file main_scene.h
 *
 * Example main scene.
 */
#pragma once
#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include <ratr0/world.h>
#include <ratr0/engine.h>

/**
 * Sets up the main scene object.
 *
 * @param engine pointer to Ratr0Engine instance
 * @return pointer to scene instance
 */
struct Ratr0Scene *setup_main_scene(Ratr0Engine *engine);

#endif /* __MAIN_SCENE_H__ */
