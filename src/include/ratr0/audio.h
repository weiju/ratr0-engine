/** @file audio.h
 *
 * Amiga audio subsystem
 */
#pragma once
#ifndef __RATR0_AUDIO_H__
#define __RATR0_AUDIO_H__

#include <ratr0/resources.h>
#define AUDIO_DEFAULT_SOUNDFX_CHANNEL (-1)
#define AUDIO_DEFAULT_SOUNDFX_VOLUME (64)
#define AUDIO_DEFAULT_SOUNDFX_PRIORITY (1)


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

/**
 * Play a sound sample.
 *
 * @param sample address of the sample to play
 * @param sound channel (0-3), -1 for any available
 */
extern void ratr0_audio_play_sound(struct Ratr0AudioSample *sample,
                                   INT8 channel);

/**
 * Play a Protracker module.
 *
 * @param mod address of the mod to play
 */
extern void ratr0_audio_play_mod(struct Ratr0AudioProtrackerMod *mod);

/**
 * Stop playing the current Protracker module.
 */
extern void ratr0_audio_stop_mod(void);

/**
 * Sets the master volume for all music channels.
 */
extern void ratr0_audio_set_master_volume(UINT8 volume);

/**
 * Pauses mod playback.
 */
extern void ratr0_audio_pause_playback(void);

/**
 * Resumes mod playback.
 */
extern void ratr0_audio_resume_playback(void);

/**
 * Toggle mod playback.
 */
extern void ratr0_audio_toggle_playback(void);

#endif /* __RATR0_AUDIO_H__ */
