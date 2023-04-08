#pragma once
#ifndef __RATR0_SCENES_H__
#define __RATR0_SCENES_H__
#include <ratr0/engine.h>

/* Scenes subsystem */
struct Ratr0SceneSystem {
    void (*shutdown)(void);
};

/**
 * Start up the scene subsystem.
 */
extern struct Ratr0SceneSystem *ratr0_scenes_startup(Ratr0Engine *);

#endif /* __RATR0_AUDIO_H__ */
