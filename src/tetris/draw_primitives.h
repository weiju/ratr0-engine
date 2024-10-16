#pragma once
#ifndef __DRAW_PRIMITIVES_H__
#define __DRAW_PRIMITIVES_H__

#include <ratr0/ratr0.h>
#include "game_data.h"

#define BOARD_X0 (112)
#define BOARD_Y0 (16)

extern void draw_piece(struct Ratr0Surface *surface,
                       struct Ratr0Surface *tiles_surface,
                       struct DrawSpec *spec, int color, int row, int col);
extern void clear_rect(struct Ratr0Surface *surface,
                       int row, int col, int num_rows, int num_cols);
extern void clear_piece(struct Ratr0Surface *surface,
                        struct DrawSpec *spec, int row, int col);

/**
 * Draw a digit on an 8 pixel aligned position
 */
extern void draw_digit(struct Ratr0Surface *surface,
                       struct Ratr0Surface *digits_surface,
                       int x, int y, char digit);

#endif /* __DRAW_PRIMITIVES_H__ */
