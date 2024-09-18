/** @file main_stage.c */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <ratr0/ratr0.h>
#include <ratr0/datastructs/queue.h>
#include <clib/graphics_protos.h>
#include <ratr0/hw_registers.h>
#include "main_stage.h"
#include "tetris_copper.h"
#include "game_data.h"

struct Ratr0CopperListInfo TETRIS_COPPER_INFO = {
    3, 5, 7, 9,
    TETRIS_COPPER_BPLCON0_INDEX, TETRIS_COPPER_BPL1MOD_INDEX,
    TETRIS_COPPER_BPL1PTH_INDEX,
    TETRIS_COPPER_SPR0PTH_INDEX, TETRIS_COPPER_COLOR00_INDEX
};

static Ratr0Engine *engine;
extern RATR0_ACTION_ID action_drop, action_move_left, action_move_right,
    action_move_down,
    action_rotate_right, action_rotate_left, action_quit;

#define DEBUG_FILE "tetris.debug"
FILE *debug_fp;

// Resources
#define BG_PATH_PAL ("tetris/assets/background_320x256x32.ts")
//#define BG_PATH_PAL ("tetris/assets/bg_320x256x32_lines.ts")
#define TILES_PATH  ("tetris/assets/tiles_32cols.ts")
#define OUTLINES_PATH  ("tetris/assets/block_outlines.spr")

struct Ratr0TileSheet background_ts, tiles_ts;
struct Ratr0Surface *backbuffer_surface, tiles_surface;

// ghost piece outline
struct Ratr0SpriteSheet outlines_sheet;
struct Ratr0HWSprite *outline_frame[9];

#define BOARD_X0 (112)
#define BOARD_Y0 (16)
#define BOARD_WIDTH (10)
#define BOARD_HEIGHT (20)

// game board, 0 means empty, if there is a piece it is block type + 1
// since the block types start at 0 as well
int gameboard0[BOARD_HEIGHT][BOARD_WIDTH];

// a quick way to determine a quick drop and the ghost piece. if the player
// piece is above the maximum height, we can drop it to the max height
int max_height0[BOARD_WIDTH];


void draw_1x3(int color, int row, int col)
{
    int x = col * 8 + BOARD_X0;
    int y = row * 8 + BOARD_Y0;
    int blit_width_words = 2;
    int shift = x % 16;
    x -= shift;

    UINT16 afwm, alwm;
    if (shift == 8) {
        afwm = 0x00ff;
        alwm = 0xffff;
        blit_width_words++;
    } else {
        afwm = 0xffff;
        alwm = 0xff00;
    }

    // 3x1 block
    ratr0_blit_ab(backbuffer_surface, &tiles_surface,
                  0, color * 32,
                  x, y,
                  0xfc, 0,
                  afwm, alwm,
                  blit_width_words, 8);
}

void draw_1x2(int color, int row, int col)
{
    int x = col * 8 + BOARD_X0;
    int y = row * 8 + BOARD_Y0;
    int blit_width_words = 1;
    int shift = x % 16;
    x -= shift;

    UINT16 afwm, alwm;
    if (shift == 8) {
        afwm = 0x00ff;
        alwm = 0xff00;
        blit_width_words++;
    } else {
        afwm = 0xffff;
        alwm = 0xffff;
    }

    // 2x1 block
    ratr0_blit_ab(backbuffer_surface, &tiles_surface,
                  0, color * 32,
                  x, y, 0xfc, 0,
                  afwm, alwm,
                  blit_width_words, 8);
}

void draw_2x2(int color, int row, int col)
{
    int x = col * 8 + BOARD_X0;
    int y = row * 8 + BOARD_Y0;
    int blit_width_words = 1;
    int shift = x % 16;
    x -= shift;

    UINT16 afwm, alwm;
    if (shift == 8) {
        afwm = 0x00ff;
        alwm = 0xff00;
        blit_width_words++;
    } else {
        afwm = 0xffff;
        alwm = 0xffff;
    }

    // 2x1 block
    ratr0_blit_ab(backbuffer_surface, &tiles_surface,
                  0, color * 32,
                  x, y, 0xfc, 0,
                  afwm, alwm,
                  blit_width_words, 16);
}

/**
 * Blits a 1x4 block
 */
void draw_1x4(int color, int row, int col)
{
    int x = col * 8 + BOARD_X0;
    int y = row * 8 + BOARD_Y0;
    int blit_width_words = 2;
    int shift = x % 16;
    x -= shift;

    UINT16 afwm = 0xffff, alwm = 0xffff;
    if (shift == 8) {
        blit_width_words++;
    }
    ratr0_blit_ab(backbuffer_surface, &tiles_surface,
                  0, color * 32,
                  x, y,
                  // we actually perform that shift !!!
                  0xfc, shift,
                  afwm, alwm,
                  blit_width_words, 8);

}

void draw_nx1(int color, int row, int col, int num_rows)
{
    int x = col * 8 + BOARD_X0;
    int y = row * 8 + BOARD_Y0;
    int shift = x % 16;
    x -= shift;

    UINT16 afwm, alwm;
    if (shift == 8) {
        afwm = 0x00ff;
        alwm = 0xffff;
    } else {
        afwm = 0xff00;
        alwm = 0xffff;
    }
    // we don't actually need a shift. We just mask either
    // the first or the second block
    ratr0_blit_ab(backbuffer_surface, &tiles_surface,
                  0, color * 32,
                  x, y, 0xfc, 0,
                  afwm, alwm,
                  1, num_rows * 8);
}

void draw_1x1(int color, int row, int col)
{
    draw_nx1(color, row, col, 1);
}
void draw_2x1(int color, int row, int col)
{
    draw_nx1(color, row, col, 2);
}
void draw_3x1(int color, int row, int col)
{
    draw_nx1(color, row, col, 3);
}
void draw_4x1(int color, int row, int col)
{
    draw_nx1(color, row, col, 4);
}

void draw_block(struct DrawSpec *spec, int color, int row, int col)
{
    for (int i = 0; i < spec->num_rects; i++) {
        switch (spec->draw_rects[i].shape) {
        case RS_1x1:
            draw_1x1(color, spec->draw_rects[i].row + row,
                     spec->draw_rects[i].col + col);
            break;
        case RS_1x2:
            draw_1x2(color, spec->draw_rects[i].row + row,
                     spec->draw_rects[i].col + col);
            break;
        case RS_1x3:
            draw_1x3(color, spec->draw_rects[i].row + row,
                     spec->draw_rects[i].col + col);
            break;
        case RS_1x4:
            draw_1x4(color, spec->draw_rects[i].row + row,
                     spec->draw_rects[i].col + col);
            break;
        case RS_2x1:
            draw_2x1(color, spec->draw_rects[i].row + row,
                     spec->draw_rects[i].col + col);
            break;
        case RS_3x1:
            draw_3x1(color, spec->draw_rects[i].row + row,
                     spec->draw_rects[i].col + col);
            break;
        case RS_4x1:
            draw_4x1(color, spec->draw_rects[i].row + row,
                     spec->draw_rects[i].col + col);
            break;
        case RS_2x2:
            draw_2x2(color, spec->draw_rects[i].row + row,
                     spec->draw_rects[i].col + col);
            break;
        default:
            break;
        }
    }
}


void clear_shape(int row, int col, int num_rows, int num_cols)
{
    int x = BOARD_X0 + col * 8;
    int y = BOARD_Y0 + row * 8;
    ratr0_blit_clear8(backbuffer_surface, x, y, num_cols * 8,
                      num_rows * 8);
}
void clear_block(struct DrawSpec *spec, int row, int col)
{
    for (int i = 0; i < spec->num_rects; i++) {
        switch (spec->draw_rects[i].shape) {
        case RS_1x1:
            clear_shape(row + spec->draw_rects[i].row,
                        col + spec->draw_rects[i].col, 1, 1);
            break;
        case RS_1x2:
            clear_shape(row + spec->draw_rects[i].row,
                        col + spec->draw_rects[i].col, 1, 2);
            break;
        case RS_1x3:
            clear_shape(row + spec->draw_rects[i].row,
                        col + spec->draw_rects[i].col, 1, 3);
            break;
        case RS_1x4:
            clear_shape(row + spec->draw_rects[i].row,
                        col + spec->draw_rects[i].col, 1, 4);
            break;
        case RS_2x1:
            clear_shape(row + spec->draw_rects[i].row,
                        col + spec->draw_rects[i].col, 2, 1);
            break;
        case RS_3x1:
            clear_shape(row + spec->draw_rects[i].row,
                        col + spec->draw_rects[i].col, 3, 1);
            break;
        case RS_4x1:
            clear_shape(row + spec->draw_rects[i].row,
                        col + spec->draw_rects[i].col, 4, 1);
            break;
        case RS_2x2:
            clear_shape(row + spec->draw_rects[i].row,
                        col + spec->draw_rects[i].col, 2, 2);
            break;
        default:
            break;
        }
    }
}

/**
 * Draw the ghost piece.
 * @param scene the current scene
 * @param outline pointer to the sprite outline
 * @param row origin row
 * @param col origin column
 */
void draw_ghost_piece(struct Ratr0Scene *scene,
                      struct SpriteOutline *outline, int row, int col)
{
    struct Ratr0HWSprite *spr0 = outline_frame[outline->framenum0];
    struct Ratr0HWSprite *spr1 = outline_frame[outline->framenum1];
    spr0->base_obj.bounds.x = BOARD_X0 + (col + outline->col0) * 8;
    spr0->base_obj.bounds.y = BOARD_Y0 + (row + outline->row0) * 8;
    spr1->base_obj.bounds.x = BOARD_X0 + (col + outline->col1) * 8;
    spr1->base_obj.bounds.y = BOARD_Y0 + (row + outline->row1) * 8;
    scene->sprites[0] = spr0;
    scene->sprites[1] = spr1;
    scene->num_sprites = 2;
}

struct PlayerState {
    struct RotationSpec *rot_spec;
    int piece, row, col;
};

struct PlayerState player_state[2] = {
    { NULL, 0, 0, 0},
    { NULL, 0, 0, 0}
};

/**
 * Since we are performing double buffering, we have a draw an a clear queue.
 * By this we can decouple logic from rendering and at the same time
 * impact the double buffer as well
 */
struct PlayerState queued_draw[1] = {
    { NULL, 0, 0, 0}
};
int num_queued_draw = 0;

struct PlayerState queued_clear[1] = {
    { NULL, 0, 0, 0}
};
int num_queued_clear = 0;

struct PlayerState NULL_PLAYER_STATE = { NULL, 0, 0, 0 };
void init_player_state(struct PlayerState *s)
{
    *s = NULL_PLAYER_STATE;
}

RATR0_QUEUE_ARR(draw_queue, struct PlayerState, 2, init_player_state, 2);
RATR0_QUEUE_ARR(clear_queue, struct PlayerState, 2, init_player_state, 2);


int cur_buffer;
int dir = 1;
int clear_count = 0;
int current_row = 0, current_col = 0;
int current_piece = PIECE_Z;
int hold_piece = -1;
// current rotation
int current_rot = 0;

#define DROP_TIMER_VALUE (40)
// rotation cooldown. We introduce a cooldown, to avoid the player
// piece rotating way too fast
#define ROTATE_COOLDOWN_TIME (10)
int rotate_cooldown = 0;
int drop_timer = DROP_TIMER_VALUE;
#define QUICKDROP_COOLDOWN_TIME (10)
int quickdrop_cooldown = 0;
#define MOVE_COOLDOWN_TIME (3)
int move_cooldown = 0;

#define PIECE_QUEUE_LEN (10)
int piece_queue[PIECE_QUEUE_LEN];
int piece_queue_idx = 0;

void dump_board(void)
{
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            fprintf(debug_fp, "%d ", gameboard0[i][j]);
        }
        fputs("\n", debug_fp);
    }
    fflush(debug_fp);
}

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
 *     the short piece right above it
 */
int get_quickdrop_row()
{
    struct Rotation *rot = &PIECE_SPECS[current_piece].rotations[current_rot].rotation;
    int min_row = BOARD_HEIGHT - 1;
    for (int t = 0; t < rot->bottom_side.num_pos; t++) {
        struct Position *pos = &rot->pos[rot->bottom_side.indexes[t]];
        for (int r = current_row + pos->y; r < BOARD_HEIGHT; r++) {
            if (gameboard0[r][current_col + pos->x] != 0) {
                int row = r - 1 - pos->y;
                if (row <= min_row) {
                    min_row = row;
                }
                break;
            }
        }
    }

    // special case for L piece rotation 3
    if (current_piece == PIECE_L && current_rot == 3 &&
        min_row == (BOARD_HEIGHT - 2)) {
        return min_row - 1;
    }
    // special case for J piece rotation 1
    if (current_piece == PIECE_J && current_rot == 1 &&
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

BOOL piece_landed(void)
{
    struct Rotation *rot = &PIECE_SPECS[current_piece].rotations[current_rot].rotation;
    for (int t = 0; t < rot->bottom_side.num_pos; t++) {
        // check every bottom tile if it has reached bottom
        struct Position *pos = &rot->pos[rot->bottom_side.indexes[t]];
        // fast out: piece is at the bottom
        if ((current_row + pos->y + 1) >= BOARD_HEIGHT) return TRUE;
        if (gameboard0[pos->y + current_row + 1][pos->x + current_col] != 0)
            return TRUE;
    }
    return FALSE;
}

/**
 * Establish the player piece on the board.
 */
void establish_piece(void)
{
    // transfer the current piece to the board
    struct Rotation *rot = &PIECE_SPECS[current_piece].rotations[current_rot].rotation;
    for (int i = 0; i < 4; i++) {
        struct Position *pos = &rot->pos[i];
        gameboard0[pos->y + current_row][pos->x + current_col] = 1;
    }
}

struct RowRange {
    int first, last;
};

/**
 * Check if there are any completed rows on the board. If the result is TRUE, the range
 * will be in the completed_rows structure.
 *
 * @param completed_rows The completed rows are returned in this structure
 * @return TRUE if there are any completed rows
 */
BOOL get_completed_rows(struct RowRange *completed_rows)
{
    BOOL result = FALSE;
    struct Rotation *rot = &PIECE_SPECS[current_piece].rotations[current_rot].rotation;
    UINT32 processed = 0; // a simple way to mark processed rows as a bit
    int min = BOARD_HEIGHT, max = 0;
    for (int i = 0; i < 4; i++) {
        struct Position *pos = &rot->pos[i];
        int row = pos->y + current_row;
        UINT32 mask = 1 << row;
        if ((mask & processed) == 0) { // no processed yet
            BOOL complete = TRUE;
            for (int j = 0; j < BOARD_WIDTH; j++) {
                if (gameboard0[row][j] == 0) {
                    complete = FALSE;
                    break;
                }
            }
            if (complete) {
                if (row < min) min = row;
                if (row > max) max = row;
                result = TRUE;
            }
            processed |= mask;
        }
    }
    completed_rows->first = min;
    completed_rows->last = max;
    return result;
}


/**
 * Random generation of all our pieces. This will restart when the end
 * of the queue is reached
 */
void init_piece_queue(void)
{
    for (int i = 0; i < PIECE_QUEUE_LEN; i++) {
        piece_queue[i] = rand() % 7;
        //piece_queue[i] = PIECE_I;
    }
    piece_queue_idx = 0;
}

void spawn_next_piece(void)
{
    current_row = 0;
    current_col = 0;
    current_piece = piece_queue[piece_queue_idx++];
    piece_queue_idx %= PIECE_QUEUE_LEN;
}

BOOL can_move_left(struct Rotation *rot)
{
    for (int i = 0; i < 4; i++) {
        int col = current_col + rot->pos[i].x;
        if (col == 0) {
            return FALSE;
        }
        int row = current_row + rot->pos[i].y;
        if (gameboard0[row][col - 1] != 0) {
            return FALSE;
        }
    }
    return TRUE;
}

BOOL can_move_right(struct Rotation *rot)
{
    for (int i = 0; i < 4; i++) {
        int col = current_col + rot->pos[i].x;
        if (col == (BOARD_WIDTH - 1)) {
            return FALSE;
        }
        int row = current_row + rot->pos[i].y;
        if (gameboard0[row][col + 1] != 0) {
            return FALSE;
        }
    }
    return TRUE;
}

struct RowRange completed_rows;

int done = 0;

void main_scene_delete_lines(struct Ratr0Scene *this_scene, UINT8 frames_elapsed)
{
    cur_buffer = ratr0_get_back_buffer()->buffernum;
    backbuffer_surface = &ratr0_get_back_buffer()->surface;
    // For now, end when the mouse was clicked. This is just for testing
    if (engine->input_system->was_action_pressed(action_quit)) {
        ratr0_engine_exit();
    }
    /*
    // delete the lines, but don't allow anything else during that time
    // we actually might not need this step since we are going to move the
    // entire contents down
    int num_deleted_rows = completed_rows.last - completed_rows.first + 1;
    clear_shape(completed_rows.first, 0,
                num_deleted_rows,
                BOARD_WIDTH);

    // move blocks above cleared lines down
    // 1. the idea is to start with (first - 1) and stop with the first
    // line that does not have any blocks. This defines the range of
    // lines that have to be shifted down by num_deleted_rows
    int topline = -1;
    for  (int i = completed_rows.last - 1; i >= 0; i--) {
        BOOL row_clear = TRUE;
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (gameboard0[i][j] != 0) {
                row_clear = FALSE;
                break;
            }
            if (row_clear) {
                // stop here
                topline = i;
                break;
            }
        }
    }
    */
    // TODO: we have the top line now. So we move the entire rectangular
    // area from topline to completed_rows.first
    // down by num_deleted_rows * 8 pixels, which means
    // a. we copy the area down
    int srcx = BOARD_X0, srcy = BOARD_Y0 + 16 * 8,
        dstx = BOARD_X0, dsty = BOARD_Y0 + 17 * 8;
    //int num_deleted_rows = completed_rows.last - completed_rows.first + 1;
    int num_deleted_rows = 3;
    int blit_width_pixels = BOARD_WIDTH * 8;
    int blit_height_pixels = num_deleted_rows * 8;
    if (done < 2) {
        // only copy once per buffer
        // reverse copying
        // this moves the entire stack above the deleted lines
        ratr0_blit_rect_simple(backbuffer_surface,
                               backbuffer_surface,
                               dstx, dsty,
                               srcx, srcy,
                               blit_width_pixels,
                               blit_height_pixels);
        // and delete the top
        clear_shape(16, 0, 1, BOARD_WIDTH);
        done++;
    }

    // b. then delete top num_deleted_rows lines
    // Since in step a we are copying to an overlapping area, we need
    // to copy backwards

    // TODO
    // when done switch back to main_scene_update
    //main_scene->update = main_scene_update;
}

struct Translate NO_TRANSLATE = {0, 0};
struct Translate *get_srs_translation(int piece, int from, int to)
{
    if (piece == PIECE_O) return &NO_TRANSLATE;

    struct Translate *tests = piece == PIECE_I ?
        WALLKICK_I[from][to] : WALLKICK_JLTSZ[from][to];
    struct Position *pos = PIECE_SPECS[piece].rotations[to].rotation.pos;

    for (int i = 0; i < NUM_WALLKICK_TESTS; i++) {
        int tx = tests[i].x;
        int ty = tests[i].y;
        BOOL ok = TRUE;
        for (int j = 0; j < 4; j++) {
            int col = current_col + pos[j].x + tx;
            int row = current_row + pos[j].y + ty;
            // check if this position is outside or occupied
            if (row >= BOARD_HEIGHT ||
                col < 0 || col >= BOARD_WIDTH ||
                (gameboard0[row][col] != 0)) {
                ok = FALSE;
                break;
            }
        }
        if (ok) {
            /*
            fprintf(debug_fp, "piece: %d, rotation: %d, successful test at i=%d row: %d col: %d\n",
                    piece, to, i, current_row, current_col);
            fflush(debug_fp);
            */
            return &tests[i];
        } else {
            /*
            fprintf(debug_fp, "piece: %d, rotation: %d, FAILED test at i=%d row: %d col: %d\n",
                    piece, to, i, current_row, current_col);
            fflush(debug_fp);
            */
        }
    }
    return NULL;
}

void main_scene_update(struct Ratr0Scene *this_scene, UINT8 frames_elapsed)
{
    cur_buffer = ratr0_get_back_buffer()->buffernum;
    backbuffer_surface = &ratr0_get_back_buffer()->surface;
    // For now, end when the mouse was clicked. This is just for testing
    if (engine->input_system->was_action_pressed(action_quit)) {
        ratr0_engine_exit();
    }
    // cooldowns
    if (move_cooldown > 0) move_cooldown--;
    if (rotate_cooldown > 0) rotate_cooldown--;
    if (quickdrop_cooldown > 0) quickdrop_cooldown--;

    // Input processing
    struct Rotation *rot = &PIECE_SPECS[current_piece].rotations[current_rot].rotation;
    if (engine->input_system->was_action_pressed(action_move_left)) {
        if (move_cooldown == 0 && can_move_left(rot)) {
            current_col--;
            move_cooldown = MOVE_COOLDOWN_TIME;
        }
    } else if (engine->input_system->was_action_pressed(action_move_right)) {
        if (move_cooldown == 0 && can_move_right(rot)) {
            current_col++;
            move_cooldown = MOVE_COOLDOWN_TIME;
        }
    } else if (engine->input_system->was_action_pressed(action_move_down)) {
        // ACCELERATE MOVE DOWN
        if (!piece_landed()) {
            current_row++;
            drop_timer = DROP_TIMER_VALUE; // reset drop timer
        }
    } else if (engine->input_system->was_action_pressed(action_drop)) {
        // QUICK DROP
        // do a quick establish on the quickdrop row
        // we can do this by setting current_row to qdr and the drop timer
        // to 0
        if (quickdrop_cooldown == 0) {
            // we also need to enqueue the current position into a clear queue
            // so we delete the graphics of the piece
            queued_clear[0].rot_spec = &PIECE_SPECS[current_piece].rotations[current_rot];
            queued_clear[0].row = current_row;
            queued_clear[0].col = current_col;
            num_queued_clear = 2;

            // now we update to the drop/establish condition
            current_row = get_quickdrop_row();
            drop_timer = 0;
            quickdrop_cooldown = QUICKDROP_COOLDOWN_TIME;
        }
    } else if (engine->input_system->was_action_pressed(action_rotate_right)) {
        // rotate right with wall kick
        if (rotate_cooldown == 0) {
            struct Translate *t = get_srs_translation(current_piece,
                                                      current_rot,
                                                      (current_rot + 1) % 4);
            if (t) {
                current_rot++;
                current_rot %= 4;
                current_row += t->y;
                current_col += t->x;
            }
            rotate_cooldown = ROTATE_COOLDOWN_TIME;
        }
    } else if (engine->input_system->was_action_pressed(action_rotate_left)) {
        // rotate left with wall kick
        if (rotate_cooldown == 0) {
            struct Translate *t = get_srs_translation(current_piece,
                                                      current_rot,
                                                      (current_rot - 1) % 4);
            if (t) {
                current_rot--;
                current_rot %= 4;
                // SRS wall kick data actually denotes y in reverse
                // so we need to subtract y
                current_row -= t->y;
                current_col += t->x;
            }
            rotate_cooldown = ROTATE_COOLDOWN_TIME;
        }
    }

    // After input

    BOOL dropped = FALSE, clear_previous = TRUE;
    // automatic drop
    if (drop_timer == 0) {
        drop_timer = DROP_TIMER_VALUE;
        if (piece_landed()) {
            struct RotationSpec *rot_spec = &PIECE_SPECS[current_piece].rotations[current_rot];
            // since we have a double buffer, we have to queue up a draw
            // for the following frame, too
            queued_draw[0] = (struct PlayerState) {rot_spec, current_piece, current_row, current_col};
            num_queued_draw = 2;

            // reset the clear positions for the piece
            for (int i = 0; i < 2; i++) {
                player_state[cur_buffer] = NULL_PLAYER_STATE;
            }
            establish_piece();
            if (get_completed_rows(&completed_rows)) {
                // delay the spawning, and delete completed lines first
                printf("detected completed rows, first: %d last: %d\n",
                       completed_rows.first,
                       completed_rows.last);

                // switch state to deleting lines mode
                this_scene->update = main_scene_delete_lines;

            } else {
                spawn_next_piece(); // spawn next piece, but don't draw yet
            }
            dropped = TRUE;
        } else {
            current_row++;
        }
    }

    // Clear queue to clean up pieces left by quick drop
    if (num_queued_clear > 0) {
        struct RotationSpec *queued_spec = queued_clear[0].rot_spec;
        clear_block(&queued_spec->draw_spec,
                    queued_clear[0].row,
                    queued_clear[0].col);
        num_queued_clear--;
    }
    // Draw queue to draw established pieces
    if (num_queued_draw > 0) {
        struct RotationSpec *queued_spec = queued_draw[0].rot_spec;
        draw_block(&queued_spec->draw_spec,
                   queued_draw[0].piece,
                   queued_draw[0].row,
                   queued_draw[0].col);
        num_queued_draw--;

        // ensure, we don't clear any lines while establishing the
        // piece in the buffer
        clear_previous = FALSE;
    }

    // This is the default draw function
    if (!dropped) {
        int qdr = get_quickdrop_row();

        // Draw new block position by clearing the old and drawing the new
        // don't draw if there are queued up draws !!!
        if (clear_previous) {
            struct RotationSpec *prev_spec = player_state[cur_buffer].rot_spec;
            clear_block(&prev_spec->draw_spec,
                        player_state[cur_buffer].row,
                        player_state[cur_buffer].col);
        }
        struct RotationSpec *rot_spec = &PIECE_SPECS[current_piece].rotations[current_rot];
        draw_block(&rot_spec->draw_spec,
                   current_piece,
                   current_row, current_col);

        // Ghost piece is drawn with sprite, no background restore needed
        draw_ghost_piece(this_scene,
                         &rot_spec->outline,
                         qdr, current_col);

        // remember state for this buffer so we can delete it
        // NOTE: maybe we can enqueue it so we have consistency
        player_state[cur_buffer] = (struct PlayerState) { rot_spec, current_piece, current_row, current_col };
        drop_timer--;
    }
}

struct Ratr0Scene *setup_main_scene(Ratr0Engine *eng)
{
    debug_fp = fopen(DEBUG_FILE, "a");

    engine = eng;
    // Use the scenes module to create a scene and run that
    struct Ratr0NodeFactory *node_factory = engine->scenes_system->get_node_factory();
    struct Ratr0Scene *main_scene = node_factory->create_scene();
    main_scene->update = main_scene_update;
    //main_scene->update = main_scene_delete_lines;

    // set new copper list
    ratr0_display_init_copper_list(tetris_copper, TETRIS_COPPER_SIZE_WORDS,
                                   &TETRIS_COPPER_INFO);

    ratr0_display_set_copperlist(tetris_copper, TETRIS_COPPER_SIZE_WORDS,
                                 &TETRIS_COPPER_INFO);

    // Load background
    engine->resource_system->read_tilesheet(BG_PATH_PAL, &background_ts);
    main_scene->backdrop = node_factory->create_backdrop(&background_ts);
    ratr0_display_set_palette(background_ts.palette,
                              32, 0);

    // Load tileset for the blocks
    engine->resource_system->read_tilesheet(TILES_PATH, &tiles_ts);
    tiles_surface.width = tiles_ts.header.width;
    tiles_surface.height = tiles_ts.header.height;
    tiles_surface.depth = tiles_ts.header.bmdepth;
    tiles_surface.is_interleaved = TRUE;
    tiles_surface.buffer = engine->memory_system->block_address(tiles_ts.h_imgdata);

    // load block outlines as sprite for the ghost piece
    engine->resource_system->read_spritesheet(OUTLINES_PATH, &outlines_sheet);
    for (int i = 0; i < outlines_sheet.header.num_sprites; i++) {
        outline_frame[i] = ratr0_create_sprite_from_sprite_sheet_frame(&outlines_sheet, i);
    }

    // initialize game board
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            gameboard0[i][j] = 0;
        }
    }
    for (int j = 0; j < BOARD_WIDTH; j++) {
        max_height0[j] = 0;
    }

    // initialize piece queue
    init_piece_queue();
    spawn_next_piece();
    return main_scene;
}
