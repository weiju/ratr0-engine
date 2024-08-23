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
typedef enum { I = 0, J, L, O, S, T, Z, CLEAR } BlockType;

/** \brief Position on the game board */
struct Position {
    int x, y;
};

/** \brief Blit source rectangle specification. */
struct BlitRect {
    int srcx, srcy, width, height;
};

/** \brief Blit specification, number and blit rectangles. */
struct BlitSpec {
    int num_blits;
    struct BlitRect blit_rects[2];
};

extern struct Position BLOCK_CONFIGS[7][4][4];
extern struct BlitSpec BLIT_SPECS[2][4];

struct BlockSpec {
    struct Position rotations[4][4];
    struct BlitSpec blit_specs[4];
};

extern struct BlockSpec I_SPEC;
extern struct BlockSpec J_SPEC;

#endif // !__GAME_DATA_H__
