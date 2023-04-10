#pragma once
#ifndef __RATR0_AMIGA_SCENES_H__
#define __RATR0_AMIGA_SCENES_H__
#include <ratr0/scenes.h>

/*
 * Amiga Scenes module.
 * This module creates scene objects that are specific to the Amiga
 * hardware. E.g. we use hardware sprites, blitter objects, dual playfields,
 * copper lists etc.
 */

/**
 * Sprites have different specifications than
 * Blitter object, as they have a special data structure, we need
 * to have a representation that accomodates for that.
 */
struct Ratr0AnimatedAmigaSprite {
    struct Ratr0AnimatedSprite2D base_obj;  // inherited base members
    //
};

extern void ratr0_amiga_scenes_startup(Ratr0Engine *eng);

extern struct Ratr0AnimatedAmigaSprite *ratr0_create_amiga_sprite(struct Ratr0TileSheet *tilesheet,
                                                                  UINT8 *frame_indexes);

#endif /* __RATR0_AMIGA_SCENES_H__ */
