#pragma once
#ifndef __GAME_DATA_H__
#define __GAME_DATA_H__

/**
 * This module defines the Tetris game's tetromino represetation.
 *
 * In addition of defining the logical arrangement of a tetromino's
 * states it stores information about how to draw its rotations
 * efficiently through blitting and sprites.
 *
 * To reduce computation effort in each frame it also
 * stores which tiles are needed to be checked when a piece moves in a
 * certain direction.
 */

/** \brief Piece types */
typedef enum {
    PIECE_I = 0, PIECE_J, PIECE_L, PIECE_O, PIECE_S, PIECE_T,
    PIECE_Z
} PieceType;

/** \brief Position on the game board */
struct Position {
    char x, y;
};

struct Translate {
    char x, y;
};

typedef enum {
    RS_1x1=0, RS_1x2, RS_1x3, RS_1x4,
    RS_2x1, RS_3x1, RS_4x1, RS_2x2
} DrawRectShape;

/** \brief a draw rectangle specification. */
struct DrawRect {
    DrawRectShape shape;
    char row, col;
};

/** \brief a sprite outline description. 2 sprites are needed for consistency */
struct SpriteOutline {
    char framenum0, row0, col0;
    char framenum1, row1, col1;
};

/** \brief Draw specification, number, position and shape of rectangles. */
struct DrawSpec {
    char num_rects;
    struct DrawRect draw_rects[2];
};

struct Side {
    char num_pos;
    char indexes[4];
};


struct Rotation {
    struct Position pos[4];
    // check these positions when dropping to the bottom
    // define left->right, top->bottom
    struct Side bottom_side;
    // define top->bottom, left->right
    //struct Side left_side;
    // define top->bottom, right->left
    //struct Side right_side;
};

struct RotationSpec {
    struct Rotation rotation;
    struct DrawSpec draw_spec;
    struct SpriteOutline outline;
};

struct PieceSpec {
    char color;
    struct RotationSpec rotations[4];
};

extern struct PieceSpec PIECE_SPECS[7];

// WALL KICK DATA
#define NUM_FROM_ROTATIONS (4)
#define NUM_TO_ROTATIONS (2)
#define NUM_WALLKICK_TESTS (5)
/**
 * Translation from -> to
 */
extern struct Translate WALLKICK_JLTSZ[NUM_FROM_ROTATIONS][NUM_TO_ROTATIONS][NUM_WALLKICK_TESTS];

extern struct Translate WALLKICK_I[NUM_FROM_ROTATIONS][NUM_TO_ROTATIONS][NUM_WALLKICK_TESTS];

#endif // !__GAME_DATA_H__
