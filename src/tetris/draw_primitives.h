#pragma once
#ifndef __DRAW_PRIMITIVES_H__
#define __DRAW_PRIMITIVES_H__

#define BOARD_X0 (112)
#define BOARD_Y0 (16)

extern void draw_1x3(int color, int row, int col);
extern void draw_1x2(int color, int row, int col);
extern void draw_2x2(int color, int row, int col);
extern void draw_1x4(int color, int row, int col);
extern void draw_nx1(int color, int row, int col, int num_rows);
extern void draw_1x1(int color, int row, int col);
extern void draw_2x1(int color, int row, int col);
extern void draw_3x1(int color, int row, int col);
extern void draw_4x1(int color, int row, int col);
extern void draw_block(struct DrawSpec *spec, int color, int row, int col);
extern void clear_shape(int row, int col, int num_rows, int num_cols);
extern void clear_block(struct DrawSpec *spec, int row, int col);


#endif /* __DRAW_PRIMITIVES_H__ */
