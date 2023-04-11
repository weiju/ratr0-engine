#pragma once
#ifndef __RATR0_AMIGA_SPRITES_H__
#define __RATR0_AMIGA_SPRITES_H__
#include <ratr0/engine.h>

/* Amiga sprite module */
extern void ratr0_amiga_sprites_startup(Ratr0Engine *eng);
extern void ratr0_amiga_sprites_shutdown(void);

extern UINT16 *ratr0_amiga_make_sprite_data(struct Ratr0TileSheet *tilesheet, UINT8 *frames, UINT8 num_frames);

#endif /* __RATR0_AMIGA_SPRITES_H__ */
