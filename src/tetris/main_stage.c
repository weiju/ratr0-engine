/** @file main_stage.c */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <ratr0/ratr0.h>
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
    action_rotate, action_quit;

// Resources
#define BG_PATH_PAL ("tetris/assets/background_320x256x32.ts")
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
    int block, rotation;
    int row, col;
};

struct PlayerState player_state[2] = {
    { 0, 0, 0},
    { 0, 0, 0}
};

int cur_buffer;
int cur_ticks = 0;
int dir = 1;
int clear_count = 0;
int current_row = 0, current_col = 0;
int current_piece = BLOCK_Z;
int hold_piece = -1;
// current rotation
int rotation = 0;

#define DROP_TIMER_VALUE (40)
// rotation cooldown. We introduce a cooldown, to avoid the player
// piece rotating way too fast
#define ROTATE_COOLDOWN_TIME (10)
int rotate_cooldown = 0;
int drop_timer = DROP_TIMER_VALUE;

#define PIECE_QUEUE_LEN (10)
int piece_queue[PIECE_QUEUE_LEN];
int piece_queue_idx = 0;


/**
 * Determines the row where the quick drop can happen at the current
 * position
 * Since we iterate from top left to right bottom, we should end up
 * with the lowest row and lowest column
 */
int get_quickdrop_row()
{
    struct Rotation *rot = &PIECE_SPECS[current_piece].rotations[rotation];
    int min_row = 19;
    struct Position *minpos = &rot->pos[rot->bottom_side.indexes[rot->bottom_side.num_pos - 1]];
    for (int t = 0; t < rot->bottom_side.num_pos; t++) {
        struct Position *pos = &rot->pos[rot->bottom_side.indexes[t]];
        for (int r = current_row + pos->y; r < 20; r++) {
            if (gameboard0[r][pos->x] != 0) {
                int row = r - pos->y;
                if (row <= min_row) {
                    min_row = row;
                    minpos = pos;
                }
                break;
            }
        }
    }
    return min_row - minpos->y;
}

int done = FALSE;
void main_scene_update(struct Ratr0Scene *this_scene, UINT8 frames_elapsed)
{
    cur_buffer = ratr0_get_back_buffer()->buffernum;
    backbuffer_surface = &ratr0_get_back_buffer()->surface;
    // For now, end when the mouse was clicked. This is just for testing
    if (engine->input_system->was_action_pressed(action_quit)) {
        ratr0_engine_exit();
    }
    if (rotate_cooldown > 0) rotate_cooldown--;
    if (drop_timer == 0) {
        drop_timer = DROP_TIMER_VALUE;
        current_row++;
    }
    if (cur_ticks == 0) {
        if (engine->input_system->was_action_pressed(action_move_left)) {
            // MOVE LEFT
            // check if all blocks can move left
            BOOL ok = TRUE;
            for (int i = 0; i < 4; i++) {
                if ((current_col + PIECE_SPECS[current_piece].rotations[rotation].pos[i].x) == 0) {
                    ok = FALSE;
                    break;
                }
            }
            if (ok) current_col--;
        } else if (engine->input_system->was_action_pressed(action_move_right)) {
            // MOVE RIGHT
            BOOL ok = TRUE;
            for (int i = 0; i < 4; i++) {
                if ((current_col + PIECE_SPECS[current_piece].rotations[rotation].pos[i].x) == 9) {
                    ok = FALSE;
                    break;
                }
            }
            if (ok) current_col++;

        } else if (engine->input_system->was_action_pressed(action_move_down)) {
            // ACCELERATE MOVE DOWN
            current_row++;
        } else if (engine->input_system->was_action_pressed(action_drop)) {
            // QUICK DROP
            int qdr = get_quickdrop_row();
            draw_block(&PIECE_SPECS[current_piece].draw_specs[rotation],
                       current_piece,
                       qdr, current_col);
        } else if (engine->input_system->was_action_pressed(action_rotate)) {
            // ROTATE
            // TODO: add wall kick
            if (rotate_cooldown == 0) {
                rotation++;
                rotation %= 4;
                rotate_cooldown = ROTATE_COOLDOWN_TIME;
            }
        }
    }
    int qdr = get_quickdrop_row();
    /*
    if (!done) {
        printf("quick drop row: %d\n", qdr);
        done = TRUE;
    }
    */

    clear_block(&PIECE_SPECS[current_piece].draw_specs[player_state[cur_buffer].rotation],
                player_state[cur_buffer].row,
                player_state[cur_buffer].col);
    draw_block(&PIECE_SPECS[current_piece].draw_specs[rotation], current_piece,
               current_row, current_col);

    // Ghost piece
    // TODO: determine lowest level to place the piece on
    draw_ghost_piece(this_scene,
                     &PIECE_SPECS[current_piece].outline[rotation],
                     qdr, current_col);

    // remember state for this buffer
    player_state[cur_buffer].rotation = rotation;
    player_state[cur_buffer].block = current_piece;
    player_state[cur_buffer].row = current_row;
    player_state[cur_buffer].col = current_col;

    // TODO:
    // We should actually be able to set the poll rate in the engine
    // or use a cooldown
    cur_ticks++;
    cur_ticks %= 2;

    drop_timer--;
}

struct Ratr0Scene *setup_main_scene(Ratr0Engine *eng)
{
    engine = eng;
    // Use the scenes module to create a scene and run that
    struct Ratr0NodeFactory *node_factory = engine->scenes_system->get_node_factory();
    struct Ratr0Scene *main_scene = node_factory->create_scene();
    main_scene->update = main_scene_update;

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
    // TODO: use srand() to initialize
    for (int i = 0; i < PIECE_QUEUE_LEN; i++) {
        piece_queue[i] = rand() % 7;
    }
    piece_queue_idx = 0;
    current_piece = piece_queue[piece_queue_idx];
    return main_scene;
}
