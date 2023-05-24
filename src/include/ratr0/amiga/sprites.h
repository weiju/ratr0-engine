/** @file sprites.h
 *
 * Amiga sprite module.
 */
#pragma once
#ifndef __RATR0_AMIGA_SPRITES_H__
#define __RATR0_AMIGA_SPRITES_H__
#include <ratr0/engine.h>

/**
 * Initialize the sprites module.
 *
 * @param engine pointer to Ratr0Engine instance
 */
extern void ratr0_amiga_sprites_startup(Ratr0Engine *engine);

/**
 * Shutdown the sprites module.
 */
extern void ratr0_amiga_sprites_shutdown(void);

/**
 * Create sprite data from a tilesheet.
 *
 * @param tilesheet pointer to tilesheet
 * @param frames array to the frames of the animation
 * @param num_frames length of the frames array
 * @return pointer to a sprite data structure
 */
extern UINT16 *ratr0_amiga_make_sprite_data(struct Ratr0TileSheet *tilesheet,
                                            UINT8 frames[], UINT8 num_frames);

/**
 * Sets sprite position.
 *
 * @param sprite_data pointer to sprite data structure
 * @param hstart sprite hstart
 * @param vstart sprite vstart
 * @param vstop sprite vstop
 */
extern void ratr0_amiga_sprites_set_pos(UINT16 *sprite_data, UINT16 hstart, UINT16 vstart, UINT16 vstop);

#endif /* __RATR0_AMIGA_SPRITES_H__ */
