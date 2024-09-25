#pragma once
#ifndef __GAME_DATA_H__
#define __GAME_DATA_H__
#include <stdio.h>
#include <ratr0/ratr0.h>

/**
 * This module defines the Tetris game's tetromino represetation and
 * game logic.
 *
 * In addition of defining the logical arrangement of a tetromino's
 * states it stores information about how to draw its rotations
 * efficiently through blitting and sprites.
 *
 * To reduce computation effort in each frame it also
 * stores which tiles are needed to be checked when a piece moves in a
 * certain direction.
 */
#define BOARD_WIDTH (10)
#define BOARD_HEIGHT (20)

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
    // These can significantly reduce the number of checks
    struct Side bottom_side;
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

struct CompletedRows {
    UINT8 count;
    UINT8 rows[4];
};

struct MoveRegion {
    int start, end;
    int move_by;
};

#define MAX_MOVE_REGIONS (2)

struct MoveRegions {
    UINT8 count;
    struct MoveRegion regions[MAX_MOVE_REGIONS];
};


// WALL KICK DATA
#define NUM_FROM_ROTATIONS (4)
#define NUM_TO_ROTATIONS (2)
#define NUM_WALLKICK_TESTS (5)
/**
 * Translation from -> to
 */
extern struct Translate WALLKICK_JLTSZ[NUM_FROM_ROTATIONS][NUM_TO_ROTATIONS][NUM_WALLKICK_TESTS];

extern struct Translate WALLKICK_I[NUM_FROM_ROTATIONS][NUM_TO_ROTATIONS][NUM_WALLKICK_TESTS];

extern struct Translate *get_srs_translation(int piece, int from, int to,
                                             int piece_row, int piece_col,
                                             int (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH]);

extern void dump_board(FILE *debug_fp,
                       int (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH]);

extern void clear_board(int (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH]);

/**
 * Determines the row where the quick drop can happen at the current
 * position
 * Since we iterate from top left to right bottom, we should end up
 * with the lowest row and lowest column
 *
 * There are cases where that fails:
 *
 *   - the L in rotation 3 and J in rotation 1 lets the piece drop one level
 *     too low when there is 1 level at the bottom and you want to hook
 *     the short piece right above it. We handle this with special cases
 */
extern int get_quickdrop_row(int piece, int rotation,
                             int piece_row, int piece_col,
                             int (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH]);


extern BOOL piece_landed(int piece, int rotation,
                         int piece_row, int piece_col,
                         int (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH]);


/**
 * Establish the player piece on the board.
 */
extern void establish_piece(int piece, int rotation,
                            int piece_row, int piece_col,
                            int (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH]);


extern BOOL get_completed_rows(struct CompletedRows *completed_rows,
                               int piece, int rotation,
                               int piece_row,
                               int (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH]);


extern BOOL can_move_right(int piece, int rotation,
                           int piece_row, int piece_col,
                           int (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH]);

extern BOOL can_move_left(int piece, int rotation,
                          int piece_row, int piece_col,
                          int (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH]);


/**
 * Get the regions moved when deleting completed_rows. The
 * result will be in move_regions, ordered from the highest numbered
 * index to lowest (bottom->top).
 *
 * @param move_regions the output
 * @param completed_rows the completed rows
 * @param gameboard the board
 * @return TRUE if it succeeded
 */
extern BOOL get_move_regions(struct MoveRegions *move_regions,
                             struct CompletedRows *completed_rows,
                             int (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH]);

extern BOOL delete_rows_from_board(struct MoveRegions *move_regions,
                                   struct CompletedRows *completed_rows,
                                   int (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH]);

#endif // !__GAME_DATA_H__
