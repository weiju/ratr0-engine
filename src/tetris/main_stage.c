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
#include "draw_primitives.h"

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

// game board, 0 means empty, if there is a piece it is block type + 1
// since the block types start at 0 as well
int gameboard0[BOARD_HEIGHT][BOARD_WIDTH];

// a quick way to determine a quick drop and the ghost piece. if the player
// piece is above the maximum height, we can drop it to the max height
int max_height0[BOARD_WIDTH];

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

/*
 * DrawQueueItem can be a shape, denoted by the piece number. A piece number
 * of -1 means only row is valid and this deletes an entire row
 */
struct DrawQueueItem {
    int piece, rotation, row, col;
    BOOL clear;  // if TRUE, clear after draw
};

struct DrawQueueItem NULL_DRAW_QUEUE_ITEM = { 0, 0, 0, 0, TRUE };
void init_draw_queue_item(struct DrawQueueItem *item)
{
    *item = NULL_DRAW_QUEUE_ITEM;
}

/**
 * For each one of the double buffers, create a draw and clear queue
 * of 4 elements
 */
RATR0_QUEUE_ARR(draw_queue, struct DrawQueueItem, 4, init_draw_queue_item, 2);
RATR0_QUEUE_ARR(clear_queue, struct DrawQueueItem, 4, init_draw_queue_item, 2);


int cur_buffer;
int dir = 1;
int clear_count = 0;
int current_row = 0, current_col = 0;
int current_piece = PIECE_Z;
int hold_piece = -1;
// current rotation
int current_rot = 0;

/** TIMERS THAT ARE CRITICAL TO GAME FEEL */
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

/**
 * Random generation of all our pieces. This will restart when the end
 * of the queue is reached
 */
void init_piece_queue(void)
{
    for (int i = 0; i < PIECE_QUEUE_LEN; i++) {
        //piece_queue[i] = rand() % 7;
        piece_queue[i] = PIECE_T;
    }
    piece_queue_idx = 0;
}

void spawn_next_piece(void)
{
    current_row = 0;
    current_col = 0;
    current_rot = 0;
    current_piece = piece_queue[piece_queue_idx++];
    piece_queue_idx %= PIECE_QUEUE_LEN;
}

struct CompletedRows completed_rows;

int done = 0;
void main_scene_update(struct Ratr0Scene *this_scene, UINT8 frames_elapsed);


// This state shifts down the blocks that are left from deleting the lines
void main_scene_shift_down_lines(struct Ratr0Scene *this_scene,
                                 UINT8 frame_elapsed) {
    // logically
    // move blocks above cleared lines down
    // 1. the idea is to start with (first - 1) and stop with the first
    // line that does not have any blocks. This defines the range of
    // lines that have to be shifted down by num_deleted_rows
    int topline = -1;
    for  (int i = completed_rows.rows[0] - 1; i >= 0; i--) {
        BOOL row_clear = TRUE;
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (gameboard0[i][j] != 0) {
                row_clear = FALSE;
                break;
            }
        }
        if (row_clear) {
            // stop here
            topline = i + 1;
            break;
        }
    }
    if (!done) {
        fprintf(debug_fp, "top line is at: %d\n", topline);
        fflush(debug_fp);
    }

    // TODO: we have the top line now. So we move the entire rectangular
    // area from topline to completed_rows.first
    // down by num_deleted_rows * 8 pixels, which means
    // a. we copy the area down
    // TODO: we actually have to shift several rows down, because
    // they are potentially disconnected
    int bottom = completed_rows.rows[0] - 1;
    int srcx = BOARD_X0, srcy = BOARD_Y0 + topline * 8,
        dstx = BOARD_X0, dsty = BOARD_Y0 + bottom * 8;
    int blit_width_pixels = BOARD_WIDTH * 8;
    //int blit_height_pixels = num_deleted_rows * 8;
    int blit_height_pixels = 8;
    if (done < 2) {
        fprintf(debug_fp, "move rect topline: %d to row: %d\n",
                topline, bottom);
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
        //clear_shape(topline, 0, num_deleted_rows, BOARD_WIDTH);
        done++;
    }

    // b. then delete top num_deleted_rows lines
    // Since in step a we are copying to an overlapping area, we need
    // to copy backwards

    // TODO
    // when done switch back to main_scene_update
    if (done >= 2) {
        this_scene->update = main_scene_update;
    }
}



/**
 * This state cleans up the artifacts from establishing the dropped piece.
 * And then it deletes the lines that were marked
 * It should actually only last for 2 frames, to affect the 2 display
 * buffers
 */
void main_scene_delete_lines(struct Ratr0Scene *this_scene, UINT8 frames_elapsed)
{
    cur_buffer = ratr0_get_back_buffer()->buffernum;
    backbuffer_surface = &ratr0_get_back_buffer()->surface;
    // For now, end when the mouse was clicked. This is just for testing
    if (engine->input_system->was_action_pressed(action_quit)) {
        ratr0_engine_exit();
    }
    // delete the lines, but don't allow anything else during that time
    // we actually might not need this step since we are going to move the
    // entire contents down
    for (int i = 0; i < completed_rows.count; i++) {
        clear_shape(completed_rows.rows[i], 0, 1, BOARD_WIDTH);
    }
    // switch to next state after 2 frames elapsed
}

/**
 * This is the main gameplay loop state. Handle input, rotate, move and place
 * blocks. The state is switched when there are completed lines
 */
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
    if (engine->input_system->was_action_pressed(action_move_left)) {
        if (move_cooldown == 0 && can_move_left(current_piece, current_rot,
                                                current_row, current_col,
                                                &gameboard0)) {
            current_col--;
            move_cooldown = MOVE_COOLDOWN_TIME;
        }
    } else if (engine->input_system->was_action_pressed(action_move_right)) {
        if (move_cooldown == 0 && can_move_right(current_piece, current_rot,
                                                 current_row, current_col,
                                                 &gameboard0)) {
            current_col++;
            move_cooldown = MOVE_COOLDOWN_TIME;
        }
    } else if (engine->input_system->was_action_pressed(action_move_down)) {
        // ACCELERATE MOVE DOWN
        if (!piece_landed(current_piece, current_rot, current_row, current_col,
                          &gameboard0)) {
            current_row++;
            drop_timer = DROP_TIMER_VALUE; // reset drop timer
        }
    } else if (engine->input_system->was_action_pressed(action_drop)) {
        // QUICK DROP
        // do a quick establish on the quickdrop row
        // we can do this by setting current_row to qdr and the drop timer
        // to 0
        if (quickdrop_cooldown == 0) {
            // we need to enqueue the current position into a clear queue
            // so we delete the graphics of the piece
            struct DrawQueueItem quick_drop_piece = {
                current_piece, current_row, current_row, current_col, FALSE
            };
            RATR0_ENQUEUE_ARR(clear_queue, 0, quick_drop_piece);
            RATR0_ENQUEUE_ARR(clear_queue, 1, quick_drop_piece);

            // now we update to the drop/establish condition
            current_row = get_quickdrop_row(current_piece, current_rot,
                                            current_row, current_col,
                                            &gameboard0);
            drop_timer = 0;
            quickdrop_cooldown = QUICKDROP_COOLDOWN_TIME;
        }
    } else if (engine->input_system->was_action_pressed(action_rotate_right)) {
        // rotate right with wall kick
        if (rotate_cooldown == 0) {
            struct Translate *t = get_srs_translation(current_piece,
                                                      current_rot,
                                                      (current_rot + 1) % 4,
                                                      current_row, current_col,
                                                      &gameboard0);
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
                                                      (current_rot - 1) % 4,
                                                      current_row, current_col,
                                                      &gameboard0);
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

    // After input, was processed, we can determine what elese is happeneing
    BOOL dropped = FALSE, clear_previous = TRUE;
    // automatic drop
    if (drop_timer == 0) {
        drop_timer = DROP_TIMER_VALUE;
        if (piece_landed(current_piece, current_rot,
                         current_row, current_col,
                         &gameboard0)) {
            // since we have a double buffer, we have to queue up a draw
            // for the following frame, too, but since this is an
            // etablished piece, don't clear it in the following frames
            struct DrawQueueItem dropped_item = (struct DrawQueueItem)
                {current_piece, current_rot, current_row, current_col, FALSE};
            RATR0_ENQUEUE_ARR(draw_queue, 0, dropped_item);
            RATR0_ENQUEUE_ARR(draw_queue, 1, dropped_item);

            establish_piece(current_piece, current_rot, current_row,
                            current_col, &gameboard0);
            if (get_completed_rows(&completed_rows, current_piece,
                                   current_rot, current_row,
                                   &gameboard0)) {
                // delay the spawning, and delete completed lines first
                // switch state to deleting lines mode
                // but make sure, we update the drawing
                this_scene->update = main_scene_delete_lines;

            } else {
                spawn_next_piece(); // spawn next piece, but don't draw yet
            }
            dropped = TRUE;
        } else {
            current_row++;
        }
    }

    // This is the default behavior if there was no drop
    if (!dropped) {
        // just draw the piece at the current position
        struct DrawQueueItem dropped_item = (struct DrawQueueItem)
            {current_piece, current_rot, current_row, current_col, TRUE};
        RATR0_ENQUEUE_ARR(draw_queue, cur_buffer, dropped_item);
        drop_timer--;
    }


    // This is the drawing section: clear all queued up clear
    // commands for the current buffer, then draw all queued up draw commands
    // for the current buffer

    // 1. Clear queue to clean up pieces left by quick drop
    while (clear_queue_num_elems[cur_buffer] > 0) {
        struct DrawQueueItem item;
        RATR0_DEQUEUE_ARR(item, clear_queue, cur_buffer);
        struct RotationSpec *queued_spec = &PIECE_SPECS[item.piece].rotations[item.rotation];
        clear_block(&queued_spec->draw_spec, item.row,
                    item.col);
    }
    // 2. draw all enqueued items
    while (draw_queue_num_elems[cur_buffer] > 0) {
        struct DrawQueueItem item;
        RATR0_DEQUEUE_ARR(item, draw_queue, cur_buffer);
        struct RotationSpec *queued_spec = &PIECE_SPECS[item.piece].rotations[item.rotation];
        draw_block(&queued_spec->draw_spec, item.piece,
                   item.row, item.col);
        if (item.clear) {
            RATR0_ENQUEUE_ARR(clear_queue, cur_buffer, item);
        }
    }
    // Ghost piece is drawn with sprite, no background restore needed
    int qdr = get_quickdrop_row(current_piece, current_rot,
                                current_row, current_col,
                                &gameboard0);
    struct RotationSpec *rot_spec = &PIECE_SPECS[current_piece].rotations[current_rot];
    draw_ghost_piece(this_scene,
                     &rot_spec->outline,
                     qdr, current_col);



}

struct Ratr0Scene *setup_main_scene(Ratr0Engine *eng)
{
    debug_fp = fopen(DEBUG_FILE, "a");

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
    // NOTE: the tilesheet remains in memory, so it could be used
    // as a backing buffer of sorts
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
    clear_board(&gameboard0);
    init_piece_queue();
    spawn_next_piece();
    return main_scene;
}
