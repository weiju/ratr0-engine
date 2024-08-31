#pragma once
#ifndef __GAME_DATA_H__
#define __GAME_DATA_H__

/**
 * This module defines the Tetris game's elementary data structures.
 * In this case, these are the block types and the board.
 *
 * For efficiency reasons, each block configuration has an associated
 * blitter specification which tells the blitter how to blit a specific
 * block in a certain rotation
 */

#define BOARD_WIDTH (10)
#define BOARD_HEIGHT (20)

/** \brief Block types */
typedef enum {
    BLOCK_I = 0, BLOCK_J, BLOCK_L, BLOCK_O, BLOCK_S, BLOCK_T,
    BLOCK_Z
} BlockType;

/** \brief Position on the game board */
struct Position {
    int x, y;
};

typedef enum {
    RS_1x1=0, RS_1x2, RS_1x3, RS_1x4,
    RS_2x1, RS_3x1, RS_4x1, RS_2x2
} DrawRectShape;

/** \brief a draw rectangle specification. */
struct DrawRect {
    DrawRectShape shape;
    int row, col;
};

/** \brief a sprite outline description. 2 sprites are needed for consistency */
struct SpriteOutline {
    int framenum0, row0, col0;
    int framenum1, row1, col1;
};

/** \brief Draw specification, number, position and shape of rectangles. */
struct DrawSpec {
    int num_rects;
    struct DrawRect draw_rects[2];
};

//extern struct Position BLOCK_CONFIGS[7][4][4];

struct BlockSpec {
    struct Position rotations[4][4];
    struct DrawSpec draw_specs[4];
    struct SpriteOutline outline[4];
};

extern struct BlockSpec BLOCK_SPECS[7];

#endif // !__GAME_DATA_H__
