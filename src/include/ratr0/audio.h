/** @file audio.h
 *
 * Audio subsystem
 */
#pragma once
#ifndef __RATR0_AUDIO_H__
#define __RATR0_AUDIO_H__
#include <ratr0/engine.h>

/**
 * Audio system interface.
 */
struct Ratr0AudioSystem {
    /**
     * Shuts down the audio system.
     */
    void (*shutdown)(void);
};

/**
 * Start up the audio subsystem.
 *
 * @param engine pointer to Ratr0Engine instance
 * @return pointer to initialized Ratr0AudioSystem instance
 */
extern struct Ratr0AudioSystem *ratr0_audio_startup(Ratr0Engine *engine);

#endif /* __RATR0_AUDIO_H__ */
