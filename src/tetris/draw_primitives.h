#pragma once
#ifndef __DRAW_PRIMITIVES_H__
#define __DRAW_PRIMITIVES_H__

#include <ratr0/ratr0.h>
#include "game_data.h"

#define BOARD_X0 (112)
#define BOARD_Y0 (16)

extern void draw_piece(struct Ratr0Surface *surface,
                       struct Ratr0Surface *tiles_surface,
                       struct DrawSpec *spec, UINT8 color, UINT8 row, UINT8 col);
extern void clear_rect(struct Ratr0Surface *surface,
                       UINT8 row, UINT8 col, UINT8 num_rows, UINT8 num_cols);
extern void clear_piece(struct Ratr0Surface *surface,
                        struct DrawSpec *spec, UINT8 row, UINT8 col);

/**
 * Draw an 8 pixel  digit on an 8 pixel aligned position
 */
extern void draw_digit8(struct Ratr0Surface *surface,
                        struct Ratr0Surface *digits_surface,
                        UINT8 digit,
                        UINT16 x, UINT16 y);

/**
 * Draw a 16 pixel digit on a 16 pixel aligned position
 */
extern void draw_digit16(struct Ratr0Surface *surface,
                         struct Ratr0Surface *digits_surface,
                         UINT8 digit,
                         UINT16 x, UINT16 y);


/**
 * Draws a preview piece to an 16 pixel aligned position.
 */
extern void draw_preview_piece(struct Ratr0Surface *surface,
                               struct Ratr0Surface *preview_surface,
                               UINT8 piece,
                               UINT16 x, UINT16 y);

#endif /* __DRAW_PRIMITIVES_H__ */
