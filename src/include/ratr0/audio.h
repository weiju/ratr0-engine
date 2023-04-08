#pragma once
#ifndef __RATR0_AUDIO_H__
#define __RATR0_AUDIO_H__

/* Audio subsystem */
struct Ratr0AudioSystem {
    void (*shutdown)(void);
};

/**
 * Start up the audio subsystem.
 */
extern struct Ratr0AudioSystem *ratr0_audio_startup(void);

#endif /* __RATR0_AUDIO_H__ */
