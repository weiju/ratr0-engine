/** @file audio.h
 *
 * Amiga audio subsystem
 */
#pragma once
#ifndef __RATR0_AMIGA_AUDIO_H__
#define __RATR0_AMIGA_AUDIO_H__

/**
 * Starts up the Amiga audio system.
 */
extern void ratr0_amiga_audio_startup(void);
/**
 * Shuts down the Amiga audio system.
 */
extern void ratr0_amiga_audio_shutdown(void);

#endif /* __RATR0_AMIGA_AUDIO_H__ */
