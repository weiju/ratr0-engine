#include "game_data.h"

// CONFIGURATIONS
// A block is a 2-dimensional array of configurations on the game board
// To optimize graphics performance, every configuration entry has
// a corresponding blitter configuration. By this means, we can turn
// 4 potential blits (8 with deletions) into a maximum of 2 blits (4
// with deletion)
// Rotation configurations of each block type
struct PieceSpec PIECE_SPECS[] = {
    // I
    {
        PIECE_I,
        {
            // rotation 0
            {
                // tiles
                {
                    { { 0, 1 }, { 1, 1 }, { 2, 1 }, { 3, 1 } },
                    { 4, {0, 1, 2, 3} }
                },
                // draw
                {1, { {RS_1x4, 1, 0} } },
                // outline
                {0, 1, 0, 1, 1, 2},
            },
            // rotation 1
            {
                // tiles
                {
                    { { 2, 0 }, { 2, 1 }, { 2, 2 }, { 2, 3 } },
                    { 1, {3, -1, -1, -1} }
                },
                // draw
                {1, { {RS_4x1, 0, 2} } },
                // outline
                {2, 0, 2, 3, 2, 2}
            },
            // rotation 2
            {
                // tiles
                {
                    { { 0, 2 }, { 1, 2 }, { 2, 2 }, { 3, 2 } },
                    { 4, {0, 1, 2, 3} }
                },
                // draw
                {1, { {RS_1x4, 2, 0} } },
                // outline
                {0, 2, 0, 1, 2, 2},
            },
            // rotation 3
            {
                // tiles
                {
                    { { 1, 0 }, { 1, 1 }, { 1, 2 }, { 1, 3 } },
                    { 1, {3, -1, -1, -1} }
                },
                // draw
                {1, { {RS_4x1, 0, 1} } },
                // outline
                {2, 0, 1, 3, 2, 1}
            }
        }
    },
    // J
    {
        PIECE_J,
        {
            // rotation 0
            {
                // tiles
                {
                    { { 0, 0 }, { 0, 1 }, { 1, 1 }, { 2, 1 } },
                    {3, {1, 2, 3, -1}}
                },
                // draw
                {2, { {RS_1x1, 0, 0}, {RS_1x3, 1, 0} } },
                // outline
                {2, 0, 0, 0, 1, 1}
            },
            // rotation 1
            {
                // tiles
                {
                    { { 1, 0 }, { 1, 1 }, { 2, 0 }, { 1, 2 } },
                    {2, {2, 3, -1, -1}}
                },
                // draw
                {2, { {RS_1x2, 0, 1}, {RS_2x1, 1, 1} } },
                // outline
                {0, 0, 1, 2, 1, 1}
            },
            // rotation 2
            {
                // tiles
                {
                    { { 0, 1 }, { 1, 1 }, { 2, 1 }, { 2, 2 } },
                    {3, {0, 1, 3, -1}}
                },
                // draw
                {2, { {RS_1x2, 1, 0}, {RS_2x1, 1, 2} } },
                // outline
                {0, 1, 0, 2, 1, 2}
            },
            // rotation 3
            {
                // tiles
                {
                    { { 0, 2 }, { 1, 0 }, { 1, 1 }, { 1, 2 } },
                    {2, {0, 3, -1, -1}}
                },
                // draw
                {2, { {RS_2x1, 0, 1}, {RS_1x2, 2, 0} } },
                // outline
                {0, 2, 0, 2, 0, 1}
            }
        }
    },
    // L
    {
        PIECE_L,
        {
            // rotation 0
            {
                // tiles
                {
                    { { 2, 0 }, { 0, 1 }, { 1, 1 }, { 2, 1 } },
                    {3, {1, 2, 3, -1}}
                },
                // draw
                {2, { {RS_1x1, 0, 2}, {RS_1x3, 1, 0} } },
                // outline
                {0, 1, 0, 2, 0, 2}
            },
            // rotation 1
            {
                // tiles
                {
                    { { 1, 0 }, { 1, 1 }, { 1, 2 }, { 2, 2 } },
                    {2, {2, 3, -1, -1}}
                },
                // draw
                {2, { {RS_2x1, 0, 1}, {RS_1x2, 2, 1} } },
                // outline
                {2, 0, 1, 0, 2, 1}
            },
            // rotation 2
            {
                // tiles
                {
                    { { 0, 1 }, { 1, 1 }, { 2, 1 }, { 0, 2 } },
                    {3, {1, 2, 3, -1}}
                },
                // draw
                {2, { {RS_2x1, 1, 0}, {RS_1x2, 1, 1} } },
                // outline
                {2, 1, 0, 0, 1, 1}
            },
            // rotation 3
            {
                // tiles
                {
                    { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 1, 2 } },
                    {2, {0, 3, -1, -1}}
                },
                // draw
                {2, { {RS_1x2, 0, 0}, {RS_2x1, 1, 1} } },
                // outline
                {0, 0, 0, 2, 1, 1}
            }
        }
    },
    // O
    // I placed blit spec into 0,0 instead of 1,1
    // because it can be drawn much more efficiently
    {
        PIECE_O,
        {
            // rotation 0
            {
                // tiles
                {
                    { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } },
                    {2, {2, 3, -1, -1}}
                },
                // draw
                {1, { {RS_2x2, 0, 0}, {0, 0, 0} } },
                // outline
                {0, 0, 0, 1, 1, 0}
            },
            // rotation 1
            {
                // tiles
                {
                    { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } },
                    {2, {2, 3, -1, -1}}
                },
                // draw
                {1, { {RS_2x2, 0, 0}, {0, 0, 0} } },
                // outline
                {0, 0, 0, 1, 1, 0}
            },
            // rotation 2
            {
                // tiles
                {
                    { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } },
                    {2, {2, 3, -1, -1}}
                },
                // draw
                {1, { {RS_2x2, 0, 0}, {0, 0, 0} } },
                // outline
                {0, 0, 0, 1, 1, 0}
            },
            // rotation 3
            {
                // tiles
                {
                    { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } },
                    {2, {2, 3, -1, -1}}
                },
                // draw
                {1, { {RS_2x2, 0, 0}, {0, 0, 0} } },
                // outline
                {0, 0, 0, 1, 1, 0}
            }
        }
    },
    // S
    {
        PIECE_S,
        {
            // rotation 0
            {
                // tiles
                {
                    { { 1, 0 }, { 2, 0 }, { 0, 1 }, { 1, 1 } },
                    {3, {1, 2, 3, -1}}
                },
                // draw
                {2, { {RS_1x2, 0, 1}, {RS_1x2, 1, 0} } },
                // outline
                {0, 0, 1, 1, 1, 0}
            },
            // rotation 1
            {
                // tiles
                {
                    { { 1, 0 }, { 1, 1 }, { 2, 1 }, { 2, 2 } },
                    {2, {1, 3, -1, -1}}
                },
                // draw
                {2, { {RS_2x1, 0, 1}, {RS_2x1, 1, 2} } },
                // outline
                {2, 0, 1, 3, 1, 2}
            },
            // rotation 2
            {
                // tiles
                {
                    { { 1, 1 }, { 2, 1 }, { 0, 2 }, { 1, 2 } },
                    {3, {1, 2, 3, -1}}
                },
                // draw
                {2, { {RS_1x2, 1, 1}, {RS_1x2, 2, 0} } },
                // outline
                {0, 1, 1, 1, 2, 0}
            },
            // rotation 3
            {
                // tiles
                {
                    { { 0, 0 }, { 0, 1 }, { 1, 1 }, { 1, 2 } },
                    {2, {1, 3, -1, -1}}
                },
                // draw
                {2, { {RS_2x1, 0, 0}, {RS_2x1, 1, 1} } },
                // outline
                {2, 0, 0, 3, 1, 1}
            }
        }
    },
    // T
    {
        PIECE_T,
        {
            // rotation 0
            {
                // tiles
                {
                    { { 1, 0 }, { 0, 1 }, { 1, 1 }, { 2, 1 } },
                    {3, {1, 2, 3, -1}}
                },
                // draw
                {2, { {RS_1x1, 0, 1}, {RS_1x3, 1, 0} } },
                // outline
                {4, 1, 0, 6, 0, 1}
            },
            // rotation 1
            {
                // tiles
                {
                    { { 1, 0 }, { 1, 1 }, { 2, 1 }, { 1, 2 } },
                    {2, {2, 3, -1, -1}}
                },
                // draw
                {2, { {RS_3x1, 0, 1}, {RS_1x1, 1, 2} } },
                // outline
                {6, 0, 1, 4, 2, 1}
            },
            // rotation 2
            {
                // tiles
                {
                    { { 0, 1 }, { 1, 1 }, { 2, 1 }, { 1, 2 } },
                    {3, {0, 2, 3, -1}}
                },
                // draw
                {2, { {RS_1x3, 1, 0}, {RS_1x1, 2, 1} } },
                // outline
                {5, 1, 0, 4, 1, 2}
            },
            // rotation 3
            {
                // tiles
                {
                    { { 0, 1 }, { 1, 0 }, { 1, 1 }, { 1, 2 } },
                    {2, {0, 3, -1, -1}}
                },
                // draw
                {2, { {RS_1x1, 1, 0}, {RS_3x1, 0, 1} } },
                // outline
                {4, 0, 1, 5, 1, 0}
            }
        }
    },
    // Z
    {
        PIECE_Z,
        {
            // rotation 0
            {
                // tiles
                {
                    { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 2, 1 } },
                    {3, {0, 2, 3, -1}}
                },
                // draw
                {2, { {RS_1x2, 0, 0}, {RS_1x2, 1, 1} } },
                // outline
                {0, 0, 0, 1, 1, 1}
            },
            // rotation 1
            {
                // tiles
                {
                    { { 2, 0 }, { 2, 1 }, { 1, 1 }, { 1, 2 } },
                    {2, {1, 3, -1, -1}}
                },
                // draw
                {2, { {RS_2x1, 1, 1}, {RS_2x1, 0, 2} } },
                // outline
                {2, 1, 1, 3, 0, 2}
            },
            // rotation 2
            {
                // tiles
                {
                    { { 0, 1 }, { 1, 1 }, { 1, 2 }, { 2, 2 } },
                    {3, {0, 2, 3, -1}}
                },
                // draw
                {2, { {RS_1x2, 1, 0}, {RS_1x2, 2, 1} } },
                // outline
                {0, 1, 0, 1, 2, 1},
            },
            // rotation 3
            {
                // tiles
                {
                    { { 1, 0 }, { 1, 1 }, { 0, 1 }, { 0, 2 } },
                    {2, {1, 3, -1, -1}}
                },
                // draw
                {2, { {RS_2x1, 1, 0}, {RS_2x1, 0, 1} } },
                // outline
                {2, 1, 0, 3, 0, 1}
            }
        }
    }
};



// KICK DATA
struct Translate WALLKICK_JLTSZ[NUM_FROM_ROTATIONS][NUM_TO_ROTATIONS][NUM_WALLKICK_TESTS] = {
    // rotation 0
    {
        {{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}}, // >> 1
        {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}} // >> 3
    },
    // rotation 1
    {
        {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}}, // >> 2
        {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}} // >> 0
    },
    // rotation 2
    {
        {{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}}, // >> 3
        {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}} // >> 1
    },
    // rotation 3
    {
        {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}, // >> 0
        {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}} // >> 2
    }
};

struct Translate WALLKICK_I[NUM_FROM_ROTATIONS][NUM_TO_ROTATIONS][NUM_WALLKICK_TESTS] = {
    // rotation 0
    {
        {{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}}, // >> 1
        {{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}} // >> 3
    },
    // rotation 1
    {
        {{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}}, // >> 2
        {{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}} // >> 0
    },
    // rotation 2
    {
        {{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}}, // >> 3
        {{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}} // >> 1
    },
    // rotation 3
    {
        {{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}}, // >> 0
        {{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}} // >> 2
    }
};

extern FILE *debug_fp;
struct Translate NO_TRANSLATE = {0, 0};
struct Translate *get_srs_translation(int piece, int from, int to,
                                      int piece_row, int piece_col,
                                      int (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH])
{
    if (piece == PIECE_O) return &NO_TRANSLATE;
    // This is the index into the kick data array, corrected
    // for the fact that the "to" array only has 2 elements
    int to_idx = (to - 1) - from % 2;

    struct Translate *tests = piece == PIECE_I ?
        WALLKICK_I[from][to_idx] : WALLKICK_JLTSZ[from][to_idx];
    struct Position *pos = PIECE_SPECS[piece].rotations[to].rotation.pos;

    for (int i = 0; i < NUM_WALLKICK_TESTS; i++) {
        int tx = tests[i].x;
        int ty = tests[i].y;
        BOOL ok = TRUE;
        for (int j = 0; j < 4; j++) {
            int col = piece_col + pos[j].x + tx;
            int row = piece_row + pos[j].y - ty;

            // check if this position is outside
            // or occupied
            if (col < 0 || row >= BOARD_HEIGHT || col >= BOARD_WIDTH ||
                ((*gameboard)[row][col] != 0)) {
                ok = FALSE;
                break;
            }
        }
        if (ok) {
            return &tests[i];
        }
    }
    return NULL;
}

void dump_board(FILE *debug_fp, int (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH])
{
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            fprintf(debug_fp, "%d ", (*gameboard)[i][j]);
        }
        fputs("\n", debug_fp);
    }
    fflush(debug_fp);
}

int get_quickdrop_row(int piece, int rotation,
                       int piece_row, int piece_col,
                       int (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH])
{
    struct Rotation *rot = &PIECE_SPECS[piece].rotations[rotation].rotation;
    int min_row = BOARD_HEIGHT - 1;
    for (int t = 0; t < rot->bottom_side.num_pos; t++) {
        struct Position *pos = &rot->pos[rot->bottom_side.indexes[t]];
        for (int r = piece_row + pos->y; r < BOARD_HEIGHT; r++) {
            if ((*gameboard)[r][piece_col + pos->x] != 0) {
                int row = r - 1 - pos->y;
                if (row <= min_row) {
                    min_row = row;
                }
                break;
            }
        }
    }

    // special case for L piece rotation 3
    if (piece == PIECE_L && rotation == 3 &&
        min_row == (BOARD_HEIGHT - 2)) {
        return min_row - 1;
    }
    // special case for J piece rotation 1
    if (piece == PIECE_J && rotation == 1 &&
        min_row == (BOARD_HEIGHT - 2)) {
        return min_row - 1;
    }

    // Handle the rest of the cases
    if (min_row == (BOARD_HEIGHT - 1)) {
        struct Position *minpos = &rot->pos[rot->bottom_side.indexes[rot->bottom_side.num_pos - 1]];
        return min_row - minpos->y;
    } else {
        return min_row;
    }
}

BOOL piece_landed(int piece, int rotation,
                  int piece_row, int piece_col,
                  int (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH])
{
    struct Rotation *rot = &PIECE_SPECS[piece].rotations[rotation].rotation;
    for (int t = 0; t < rot->bottom_side.num_pos; t++) {
        // check every bottom tile if it has reached bottom
        struct Position *pos = &rot->pos[rot->bottom_side.indexes[t]];
        // fast out: piece is at the bottom
        if ((piece_row + pos->y + 1) >= BOARD_HEIGHT) return TRUE;
        if ((*gameboard)[pos->y + piece_row + 1][pos->x + piece_col] != 0)
            return TRUE;
    }
    return FALSE;
}

/**
 * Establish the player piece on the board.
 */
void establish_piece(int piece, int rotation,
                     int piece_row, int piece_col,
                     int (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH])
{
    // transfer the current piece to the board
    struct Rotation *rot = &PIECE_SPECS[piece].rotations[rotation].rotation;
    for (int i = 0; i < 4; i++) {
        struct Position *pos = &rot->pos[i];
        (*gameboard)[pos->y + piece_row][pos->x + piece_col] = 1;
    }
}

/**
 * Check if there are any completed rows on the board based on the
 * rows that a game piece was placed in. If the result is TRUE, the
 * completed row numbers will be in the completed_rows structure.
 *
 * @param completed_rows The completed rows are returned in this structure
 * @param piece The piece being placed
 * @param rotation rotation of the piece
 * @param piece_row row position of the piece
 * @return TRUE if there are any completed rows
 */
BOOL get_completed_rows(struct CompletedRows *completed_rows,
                        int piece, int rotation,
                        int piece_row,
                        int (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH])
{
    BOOL result = FALSE;
    struct Rotation *rot = &PIECE_SPECS[piece].rotations[rotation].rotation;
    UINT32 processed = 0; // a simple way to mark processed rows as a bit
    completed_rows->count = 0;

    for (int i = 0; i < 4; i++) {
        struct Position *pos = &rot->pos[i];
        int row = pos->y + piece_row;
        UINT32 mask = 1 << row;
        if ((mask & processed) == 0) { // row not processed yet
            BOOL complete = TRUE;
            for (int j = 0; j < BOARD_WIDTH; j++) {
                if ((*gameboard)[row][j] == 0) {
                    complete = FALSE;
                    break;
                }
            }
            if (complete) {
                completed_rows->rows[completed_rows->count++] =  row;
                result = TRUE;
            }
            processed |= mask;
        }
    }
    return result;
}


BOOL can_move_left(int piece, int rotation,
                   int piece_row, int piece_col,
                   int (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH])
{
    struct Rotation *rot = &PIECE_SPECS[piece].rotations[rotation].rotation;
    for (int i = 0; i < 4; i++) {
        int col = piece_col + rot->pos[i].x;
        if (col == 0) {
            return FALSE;
        }
        int row = piece_row + rot->pos[i].y;
        if ((*gameboard)[row][col - 1] != 0) {
            return FALSE;
        }
    }
    return TRUE;
}

BOOL can_move_right(int piece, int rotation,
                    int piece_row, int piece_col,
                    int (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH])
{
    struct Rotation *rot = &PIECE_SPECS[piece].rotations[rotation].rotation;
    for (int i = 0; i < 4; i++) {
        int col = piece_col + rot->pos[i].x;
        if (col == (BOARD_WIDTH - 1)) {
            return FALSE;
        }
        int row = piece_row + rot->pos[i].y;
        if ((*gameboard)[row][col + 1] != 0) {
            return FALSE;
        }
    }
    return TRUE;
}

void clear_board(int (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH])
{
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            (*gameboard)[i][j] = 0;
        }
    }
}
