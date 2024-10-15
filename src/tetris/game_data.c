#include "game_data.h"

// CONFIGURATIONS
// A block is a 2-dimensional array of configurations on the game board
// To optimize graphics performance, every configuration entry has
// a corresponding blitter configuration. By this means, we can turn
// 4 potential blits (8 with deletions) into a maximum of 2 blits (4
// with deletion)
// Rotation configurations of each block type
struct PieceSpec PIECE_SPECS[NUM_PIECES] = {
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
struct Translate *get_srs_translation(struct PieceState *from, UINT8 to,
                                      UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH])
{
    if (from->piece == PIECE_O) return &NO_TRANSLATE;
    // This is the index into the kick data array, corrected
    // for the fact that the "to" array only has 2 elements
    UINT8 to_idx = ((to - from->rotation) == -1) ? 1 : 0;

    struct Translate (*tests)[NUM_WALLKICK_TESTS] = from->piece == PIECE_I ?
        &(WALLKICK_I[from->rotation][to_idx]) :
        &(WALLKICK_JLTSZ[from->rotation][to_idx]);

    struct Position *pos = PIECE_SPECS[from->piece].rotations[to].rotation.pos;

    for (int i = 0; i < NUM_WALLKICK_TESTS; i++) {
        int tx = (*tests)[i].x;
        int ty = (*tests)[i].y;
        BOOL ok = TRUE;
        for (int j = 0; j < 4; j++) {
            int col = from->col + pos[j].x + tx;
            int row = from->row + pos[j].y - ty;

            // check if this position is outside
            // or occupied
            if (col < 0 || row >= BOARD_HEIGHT || col >= BOARD_WIDTH ||
                ((*gameboard)[row][col] != 0)) {
                ok = FALSE;
                break;
            }
        }
        if (ok) {
            return &((*tests)[i]);
        }
    }
    return NULL;
}

void dump_board(FILE *debug_fp, UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH])
{
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            fprintf(debug_fp, "%u ", (*gameboard)[i][j]);
        }
        fputs("\n", debug_fp);
    }
    fflush(debug_fp);
}

int get_quickdrop_row(struct PieceState *piece,
                      UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH])
{
    struct Rotation *rot = &PIECE_SPECS[piece->piece].rotations[piece->rotation].rotation;
    int min_row = BOARD_HEIGHT - 1;
    for (int t = 0; t < rot->bottom_side.num_pos; t++) {
        struct Position *pos = &rot->pos[rot->bottom_side.indexes[t]];
        for (int r = piece->row + pos->y; r < BOARD_HEIGHT; r++) {
            if ((*gameboard)[r][piece->col + pos->x] != 0) {
                int row = r - 1 - pos->y;
                if (row <= min_row) {
                    min_row = row;
                }
                break;
            }
        }
    }

    // special case for L piece rotation 3
    if (piece->piece == PIECE_L && piece->rotation == 3 &&
        min_row == (BOARD_HEIGHT - 2)) {
        return min_row - 1;
    }
    // special case for J piece rotation 1
    if (piece->piece == PIECE_J && piece->rotation == 1 &&
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

BOOL piece_landed(struct PieceState *piece,
                  UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH])
{
    struct Rotation *rot = &PIECE_SPECS[piece->piece].rotations[piece->rotation].rotation;
    for (int t = 0; t < rot->bottom_side.num_pos; t++) {
        // check every bottom tile if it has reached bottom
        struct Position *pos = &rot->pos[rot->bottom_side.indexes[t]];
        // fast out: piece is at the bottom
        if ((piece->row + pos->y + 1) >= BOARD_HEIGHT) return TRUE;
        if ((*gameboard)[pos->y + piece->row + 1][pos->x + piece->col] != 0)
            return TRUE;
    }
    return FALSE;
}

/**
 * Establish the player piece on the board.
 */
void establish_piece(struct PieceState *piece,
                     UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH])
{
    // transfer the current piece to the board
    struct Rotation *rot = &PIECE_SPECS[piece->piece].rotations[piece->rotation].rotation;
    for (int i = 0; i < 4; i++) {
        struct Position *pos = &rot->pos[i];
        (*gameboard)[pos->y + piece->row][pos->x + piece->col] = 1;
    }
}

/**
 * Check if there are any completed rows on the board based on the
 * rows that a game piece was placed in. If the result is TRUE, the
 * completed row numbers will be in the completed_rows structure, ordered
 * from highest to lowest (which means lowest row number first and
 * highest row number last)
 *
 * @param completed_rows The completed rows are returned in this structure
 * @param piece The piece being placed
 * @param rotation rotation of the piece
 * @param piece_row row position of the piece
 * @return number of completed rows
 */
UINT8 get_completed_rows(struct CompletedRows *completed_rows,
                         struct PieceState *piece,
                         UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH])
{
    BOOL result = FALSE, complete;
    struct Rotation *rot = &PIECE_SPECS[piece->piece].rotations[piece->rotation].rotation;
    UINT32 processed = 0; // a simple way to mark processed rows as a bit
    completed_rows->count = 0;

    for (int i = 0; i < 4; i++) {
        struct Position *pos = &rot->pos[i];
        int row = pos->y + piece->row;
        UINT32 mask = 1 << row;
        if ((mask & processed) == 0) { // row not processed yet
            complete = TRUE;
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
    return completed_rows->count;
}

BOOL can_move_left(struct PieceState *piece,
                   UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH])
{
    struct Rotation *rot = &PIECE_SPECS[piece->piece].rotations[piece->rotation].rotation;
    for (int i = 0; i < 4; i++) {
        int col = piece->col + rot->pos[i].x;
        if (col == 0) {
            return FALSE;
        }
        int row = piece->row + rot->pos[i].y;
        if ((*gameboard)[row][col - 1] != 0) {
            return FALSE;
        }
    }
    return TRUE;
}

BOOL can_move_right(struct PieceState *piece,
                    UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH])
{
    struct Rotation *rot = &PIECE_SPECS[piece->piece].rotations[piece->rotation].rotation;
    for (int i = 0; i < 4; i++) {
        int col = piece->col + rot->pos[i].x;
        if (col == (BOARD_WIDTH - 1)) {
            return FALSE;
        }
        int row = piece->row + rot->pos[i].y;
        if ((*gameboard)[row][col + 1] != 0) {
            return FALSE;
        }
    }
    return TRUE;
}

void clear_board(UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH])
{
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            (*gameboard)[i][j] = 0;
        }
    }
}

BOOL _get_move_region_above(struct MoveRegion *move_region,
                            UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH],
                            int row, int min_row)
{
    BOOL result = FALSE;
    move_region->end = row;
    for (int i = row; i >= min_row; i--) {
        BOOL empty_row = TRUE;
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if ((*gameboard)[i][j] != 0) {
                empty_row = FALSE;
                break;
            }
        }
        if (empty_row) break;
        move_region->start = i;
        result = TRUE;
    }
    return result;
}

BOOL get_move_regions(struct MoveRegions *move_regions,
                      struct CompletedRows *completed_rows,
                      UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH])
{
    if (completed_rows->count == 1 || completed_rows->count == 4) {
        // there is at most one region to move and it's above the first
        // completed row since completed rows go from top to bottom
        BOOL res = _get_move_region_above(&move_regions->regions[0],
                                          gameboard,
                                          completed_rows->rows[0] - 1, 0);
        if (res) {
            move_regions->regions[0].move_by = completed_rows->count;
            move_regions->count = 1;
        } else {
            move_regions->count = 0;
        }
    } else if (completed_rows->count == 2) {
        // the 2 deleted rows are either connected
        // or there is a gap
        int diff = completed_rows->rows[1] - completed_rows->rows[0];
        if (diff == 1) {
            // connected -> 0 or 1 ranges
            BOOL res = _get_move_region_above(&move_regions->regions[0],
                                              gameboard,
                                              completed_rows->rows[0] - 1, 0);
            if (res) {
                move_regions->count = 1;
                move_regions->regions[0].move_by = completed_rows->count;
            } else {
                move_regions->count = 0;
            }
        } else {
            // gap of height "diff" -> 1 or 2 regions
            BOOL res;
            // 1. get the gap region first
            move_regions->count = 1;
            res = _get_move_region_above(&move_regions->regions[0],
                                         gameboard,
                                         completed_rows->rows[1] - 1,
                                         completed_rows->rows[0] + 1);
            move_regions->regions[0].move_by = diff - 1;
            // 2. get the above region next
            res = _get_move_region_above(&move_regions->regions[1],
                                         gameboard,
                                         completed_rows->rows[0] - 1, 0);
            if (res) {
                move_regions->count++;
                move_regions->regions[1].move_by = diff;
            }
        }
    } else if (completed_rows->count == 3) {
        // most complicated case
        // the 3 deleted rows are either connected
        // or there are 2 connected and one disconnected
        int diff1 = completed_rows->rows[1] - completed_rows->rows[0];
        int diff2 = completed_rows->rows[2] - completed_rows->rows[1];
        if (diff1 == 1 && diff2 == 1) {
            // connected -> 0 or 1 regions
            BOOL res = _get_move_region_above(&move_regions->regions[0],
                                              gameboard,
                                              completed_rows->rows[0] - 1, 0);
            if (res) {
                move_regions->count = 1;
                move_regions->regions[0].move_by = completed_rows->count;
            } else {
                move_regions->count = 0;
            }
        } else if (diff1 == 1) {
            // top and second  are connected
            // -> 1 or 2 regions and the gap is height 1
            // 1. get the gap region first, it is fixed, so no calculation needed
            move_regions->count = 1;
            move_regions->regions[0].start = move_regions->regions[0].end
                = completed_rows->rows[2] - 1;
            move_regions->regions[0].move_by = 1;
            BOOL res = _get_move_region_above(&move_regions->regions[1],
                                             gameboard,
                                             completed_rows->rows[0] - 1, 0);
            if (res) {
                move_regions->count++;
                move_regions->regions[1].move_by = 3;
            }
        } else {
            // second and bottom are connected
            // -> 1 or 2 regions and the gap is height 1
            // 1. get the gap region first, it is fixed, so no calculation needed
            move_regions->count = 1;
            move_regions->regions[0].start = move_regions->regions[0].end
                = completed_rows->rows[1] - 1;
            move_regions->regions[0].move_by = 2;
            BOOL res = _get_move_region_above(&move_regions->regions[1],
                                              gameboard,
                                              completed_rows->rows[0] - 1, 0);
            if (res) {
                move_regions->count++;
                move_regions->regions[1].move_by = 3;
            }
        }
    }
    return TRUE;
}

/**
 * Move the specified region. We do this by moving rows by
 * (region->move_by) from bottom to top
 *
 * @parame region region to move
 * @param gameboard the game board to modify
 */
void _move_region(struct MoveRegion *region,
                  UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH])
{
    int move_by = region->move_by;
    for (int i = region->end; i >= region->start; i--) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            (*gameboard)[i + move_by][j] = (*gameboard)[i][j];
        }
    }
}

/**
 * Logically delete the rows of the board by clearing the deleted lines and
 * dropping the blocks above them down as far as possible
 * Since we already computed MoveRegions, we can use that information to
 * move the data from these regions and delete
 * completed_rows.count rows on the top of the move regions
 */
BOOL delete_rows_from_board(struct MoveRegions *move_regions,
                            struct CompletedRows *completed_rows,
                            UINT8 (*gameboard)[BOARD_HEIGHT][BOARD_WIDTH])
{
    int delete_start = completed_rows->rows[0];

    // we have at least 1 region, so move them in order, which
    // means from bottom to top
    int num_regions = move_regions->count;
    if (num_regions > 0) {
        delete_start = move_regions->regions[num_regions - 1].start;
        for (int i = 0; i < num_regions; i++) {
            _move_region(&move_regions->regions[i], gameboard);
        }
    }

    // Delete the top rows: if there are no move_regions, then take the
    // first completed_rows.count rows, starting from the first one
    // otherwise start see if the first move region starts before
    // completed_rows and delete from the highest row of that
    for (int i = 0; i < completed_rows->count; i++) {
        int row = delete_start + i;
        for (int j = 0; j < BOARD_WIDTH; j++) {
            (*gameboard)[row][j] = 0;
        }
    }
    return TRUE;
}

void reset_player_stats(struct PlayerStats *player_stats)
{
    player_stats->difficulty_level = 1;
    player_stats->level_cleared_rows = 0;
    player_stats->total_cleared_rows = 0;
    player_stats->score = 0;
    player_stats->drop_timer_value = DROP_TIMER_VALUE;
    player_stats->seconds_played = 0;
}

void score_soft_drop(struct PlayerStats *player_stats)
{
    player_stats->score++;
}

void score_hard_drop(struct PlayerStats *player_stats, int num_rows)
{
    player_stats->score += 2 * num_rows;
}

BOOL score_rows_cleared(struct PlayerStats *player_stats, int num_rows)
{
    switch (num_rows) {
    case 1:
        player_stats->score += 100 * player_stats->difficulty_level;
        break;
    case 2:
        player_stats->score += 300 * player_stats->difficulty_level;
        break;
    case 3:
        player_stats->score += 500 * player_stats->difficulty_level;
        break;
    case 4:
        player_stats->score += 800 * player_stats->difficulty_level;
        break;
    default:
        break;
    }
    player_stats->level_cleared_rows += num_rows;
    player_stats->total_cleared_rows += num_rows;
    if (player_stats->level_cleared_rows >= 10) {
        player_stats->difficulty_level++;

        // make drop faster, but never make it negative
        player_stats->drop_timer_value -= 5;
        if (player_stats->drop_timer_value < 2) {
            player_stats->drop_timer_value = 2;
        }
        player_stats->level_cleared_rows -= 10;
        return TRUE;
    } else {
        return FALSE;
    }
}
