/** @file main_stage.c */
#include <stdio.h>
#include <string.h>

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

struct Ratr0TileSheet background_ts, tiles_ts;
struct Ratr0Surface *backbuffer_surface, tiles_surface;

#define BOARD_X0 (112)
#define BOARD_Y0 (16)


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
// current block
int current_block = BLOCK_Z;
// current rotation
int rotation = 0;

// rotation cooldown. We introduce a cooldown, to avoid the player
// piece rotating way too fast
#define ROTATE_COOLDOWN_TIME (10)
int rotate_cooldown = 0;

void main_scene_update(struct Ratr0Scene *this_scene, UINT8 frames_elapsed)
{
    cur_buffer = ratr0_get_back_buffer()->buffernum;
    backbuffer_surface = &ratr0_get_back_buffer()->surface;
    // For now, end when the mouse was clicked. This is just for testing
    if (engine->input_system->was_action_pressed(action_quit)) {
        ratr0_engine_exit();
    }
    if (rotate_cooldown > 0) rotate_cooldown--;
    if (cur_ticks == 0) {
        if (engine->input_system->was_action_pressed(action_move_left)) {
            current_col--;
            if (current_col < 0) current_col = 0;
        } else if (engine->input_system->was_action_pressed(action_move_right)) {
            current_col++;
            if (current_col > 6) current_col = 6;
        } else if (engine->input_system->was_action_pressed(action_move_down)) {
            current_row++;
            if (current_row > 10) current_row = 10;
        } else if (engine->input_system->was_action_pressed(action_drop)) {
        } else if (engine->input_system->was_action_pressed(action_rotate)) {
            if (rotate_cooldown == 0) {
                rotation++;
                rotation %= 4;
                rotate_cooldown = ROTATE_COOLDOWN_TIME;
            }
        }
    }
    clear_block(&BLOCK_SPECS[current_block].draw_specs[player_state[cur_buffer].rotation],
                player_state[cur_buffer].row,
                player_state[cur_buffer].col);
    draw_block(&BLOCK_SPECS[current_block].draw_specs[rotation], 0, current_row, current_col);

    // remember state for this buffer
    player_state[cur_buffer].rotation = rotation;
    player_state[cur_buffer].block = current_block;
    player_state[cur_buffer].row = current_row;
    player_state[cur_buffer].col = current_col;

    // TODO:
    // We should actually be able to set the poll rate in the engine
    // or use a cooldown
    cur_ticks++;
    cur_ticks %= 2;
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

    return main_scene;
}
