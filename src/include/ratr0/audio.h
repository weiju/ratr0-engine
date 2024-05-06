/** @file audio.h
 *
 * Amiga audio subsystem
 */
#pragma once
#ifndef __RATR0_AUDIO_H__
#define __RATR0_AUDIO_H__

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
 * Starts up the Amiga audio system.
 */
extern struct Ratr0AudioSystem *ratr0_audio_startup(void);
/**
 * Shuts down the Amiga audio system.
 */
extern void ratr0_audio_shutdown(void);

#endif /* __RATR0_AUDIO_H__ */
