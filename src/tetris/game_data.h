#pragma once
#ifndef __GAME_DATA_H__
#define __GAME_DATA_H__
#include <stdio.h>
#include <ratr0/ratr0.h>
#include <ratr0/datastructs/queue.h>

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
#define BOARD_HEIGHT (22)
#define NUM_PIECES (7)
#define NUM_ROTATIONS (4)
#define NUM_BLOCKS_PER_PIECE (4)
#define MAX_COMPLETED_ROWS (4)
#define MAX_MOVE_REGIONS (2)
#define PIECE_QUEUE_LEN (40)

#define DRAW_PIECE_QUEUE_LEN (10)
#define CLEAR_ROW_QUEUE_LEN (10)
#define MOVE_QUEUE_LEN (10)
#define PREVIEW_QUEUE_LEN (4)

#define NUM_DISPLAY_BUFFERS (2)

/** \brief Piece types */
#define PIECE_UNDEFINED (-1)
typedef enum {
    PIECE_I = 0, PIECE_J, PIECE_L, PIECE_O, PIECE_S, PIECE_T,
    PIECE_Z
} PieceType;

/** \brief Position on the game board */
struct Position {
    INT8 x, y;
};

struct Translate {
    INT8 x, y;
};

struct PieceState {
    INT8 piece, rotation, row, col;
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
    INT8 framenum0, row0, col0;
    INT8 framenum1, row1, col1;
};

/** \brief Draw specification, number, position and shape of rectangles. */
struct DrawSpec {
    INT8 num_rects;
    struct DrawRect draw_rects[2];
};

struct Side {
    INT8 num_pos;
    INT8 indexes[NUM_BLOCKS_PER_PIECE];
};

struct Rotation {
    struct Position pos[NUM_BLOCKS_PER_PIECE];

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
    struct RotationSpec rotations[NUM_ROTATIONS];
};

extern struct PieceSpec PIECE_SPECS[NUM_PIECES];

struct CompletedRows {
    UINT8 count;
    UINT8 rows[MAX_COMPLETED_ROWS];
};

struct MoveRegion {
    UINT8 start, end;
    UINT8 move_by;
};

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

extern struct Translate *get_srs_translation(struct PieceState *from, UINT8 to,
                                             UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH]);

extern void dump_board(FILE *debug_fp,
                       UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH]);

extern void clear_board(UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH]);

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
extern int get_quickdrop_row(struct PieceState *piece,
                             UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH]);


extern BOOL piece_landed(struct PieceState *piece,
                         UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH]);


/**
 * Establish the player piece on the board.
 */
extern void establish_piece(struct PieceState *piece,
                            UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH]);

extern UINT8 get_completed_rows(struct CompletedRows *completed_rows,
                                struct PieceState *piece,
                                UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH]);


extern BOOL can_move_right(struct PieceState *piece,
                           UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH]);

extern BOOL can_move_left(struct  PieceState *piece,
                          UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH]);


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
                             UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH]);

extern BOOL delete_rows_from_board(struct MoveRegions *move_regions,
                                   struct CompletedRows *completed_rows,
                                   UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH]);



/**
 * PLAYER STATS OPERATIONS
 * Scoring rules and level ups are implemented here.
 * Points are
 *   - +1 for each accelerated move down
 *   - +2 * row for each hard drop
 *   - +100 * level for 1 line cleared
 *   - +300 * level for 2 lines cleared
 *   - +500 * level for 3 lines cleared
 *   - +800 * level for 4 lines cleared (Tetris)
 *   - combo: 50 * combo count * level
 *
 * The Level increases each 10 cleared lines

 * TODO: advanced scoring (https://tetris.wiki/Scoring)
 */
#define DROP_TIMER_VALUE (40)

struct PlayerState {
    UINT8 difficulty_level, level_cleared_rows;
    UINT8 drop_timer_value;
    UINT32 total_cleared_rows;
    UINT32 score;
    UINT32 seconds_played;
    INT8 hold;  // the held piece, -1 = no hold
    BOOL can_swap_hold;
};

extern void reset_player_state(struct PlayerState *player_state);
extern void score_soft_drop(struct PlayerState *player_state);
extern void score_hard_drop(struct PlayerState *player_state, int num_rows);
/**
 * returns true if level increased
 */
extern BOOL score_rows_cleared(struct PlayerState *player_state, int num_rows);

/**
 * Random generation of all our pieces. This will restart when the end
 * of the queue is reached
 */
extern void init_piece_queue(UINT8 (*piece_queue)[PIECE_QUEUE_LEN]);


// DRAW, CLEAR AND MOVE QUEUES

struct PieceQueueItem {
    UINT8 piece, rotation, row, col;
    BOOL clear; // if TRUE, clear after draw
};
struct RowQueueItem {
    UINT8 row, num_rows;
};

RATR0_QUEUE_ARR_DEF(draw_piece_queue, struct PieceQueueItem,
                    DRAW_PIECE_QUEUE_LEN,
                    NUM_DISPLAY_BUFFERS)

RATR0_QUEUE_ARR_DEF(clear_piece_queue, struct PieceQueueItem,
                    DRAW_PIECE_QUEUE_LEN,
                    NUM_DISPLAY_BUFFERS)

RATR0_QUEUE_ARR_DEF(clear_row_queue, struct RowQueueItem,
                    CLEAR_ROW_QUEUE_LEN,
                    NUM_DISPLAY_BUFFERS)

/**
 * The Move queue is to store the actions to move regions of block rows
 * after lines where deleted
 */
struct MoveQueueItem {
    int from, to, num_rows;
};

RATR0_QUEUE_ARR_DEF(move_queue, struct MoveQueueItem, MOVE_QUEUE_LEN,
                    NUM_DISPLAY_BUFFERS)


enum {
    PREVIEW_TYPE_NEXT = 0, PREVIEW_TYPE_HOLD
};

struct PreviewQueueItem {
    UINT8 preview_type, piece, position;
};


RATR0_QUEUE_ARR_DEF(preview_queue, struct PreviewQueueItem, PREVIEW_QUEUE_LEN,
                    NUM_DISPLAY_BUFFERS)


/*
struct ScoreDigit {
    UINT8 digit;
    UINT8 rpos;
};
*/


#endif // !__GAME_DATA_H__
