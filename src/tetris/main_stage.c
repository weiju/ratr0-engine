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
struct Ratr0Surface tiles_surface;

// ghost piece outline
struct Ratr0SpriteSheet outlines_sheet;
struct Ratr0HWSprite *outline_frame[9];

// game board, 0 means empty, if there is a piece it is block type + 1
// since the block types start at 0 as well
int gameboard0[BOARD_HEIGHT][BOARD_WIDTH];

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

#define NUM_DISPLAY_BUFFERS (2)
#define DRAW_QUEUE_LEN (4)
enum {
    DRAW_TYPE_PIECE = 0, DRAW_TYPE_ROWS
};

/*
 * DrawQueueItem can be a shape, denoted by the piece number. A piece number
 * of -1 means only row is valid and this deletes an entire row
 */
struct DrawPiece {
    int piece, rotation, row, col;
    BOOL clear; // if TRUE, clear after draw
};
struct ClearRows {
    int row, num_rows;
};
struct DrawQueueItem {
    int draw_type; // piece or rows
    union {
        struct DrawPiece piece;
        struct ClearRows rows;
    } item;
};

struct DrawQueueItem NULL_DRAW_QUEUE_ITEM = {
    DRAW_TYPE_PIECE, {
        0, 0, 0, 0,
        TRUE
    }
};
void init_draw_queue_item(struct DrawQueueItem *item)
{
    *item = NULL_DRAW_QUEUE_ITEM;
}

/**
 * For each one of the double buffers, create a draw and clear queue
 * of 4 elements
 */
RATR0_QUEUE_ARR(draw_queue, struct DrawQueueItem, DRAW_QUEUE_LEN,
                init_draw_queue_item, NUM_DISPLAY_BUFFERS)
RATR0_QUEUE_ARR(clear_queue, struct DrawQueueItem, DRAW_QUEUE_LEN,
                init_draw_queue_item, NUM_DISPLAY_BUFFERS)

/**
 * The Move queue is to store the actions to move regions of block rows
 * after lines where deleted
 */
struct MoveQueueItem {
    int from, to, num_rows;
};
struct MoveQueueItem NULL_MOVE_QUEUE_ITEM = { 0, 0, 0 };
void init_move_queue_item(struct MoveQueueItem *item)
{
    *item = NULL_MOVE_QUEUE_ITEM;
}
RATR0_QUEUE_ARR(move_queue, struct MoveQueueItem, DRAW_QUEUE_LEN,
                init_move_queue_item, NUM_DISPLAY_BUFFERS)

int current_row = 0, current_col = 0;
int current_piece = PIECE_Z;
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
        piece_queue[i] = rand() % 7;
        //piece_queue[i] = PIECE_T;
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

/**
 * This is the drawing section: clear all queued up clear
 * commands for the current buffer, then draw all queued up draw commands
 * for the current buffer
 */
void process_blit_queues(void)
{
    struct DrawQueueItem item;
    struct RotationSpec *queued_spec;
    struct Ratr0Surface *backbuffer_surface = &ratr0_get_back_buffer()->surface;
    int cur_buffer = ratr0_get_back_buffer()->buffernum;
    // 1. Clear queue to clean up pieces from the previous render pass
    while (clear_queue_num_elems[cur_buffer] > 0) {
        RATR0_DEQUEUE_ARR(item, clear_queue, cur_buffer);
        if (item.draw_type == DRAW_TYPE_ROWS) {
            // clear a row
            fprintf(debug_fp, "clear row %d on buffer %d\n",
                    item.item.rows.row,
                    cur_buffer);
            clear_rect(backbuffer_surface, item.item.rows.row, 0, 1, BOARD_WIDTH);
        } else {
            queued_spec = &PIECE_SPECS[item.item.piece.piece].rotations[item.item.piece.rotation];
            clear_piece(backbuffer_surface,
                        &queued_spec->draw_spec, item.item.piece.row,
                        item.item.piece.col);
        }
        fflush(debug_fp);
    }
    // 2. draw all enqueued items
    while (draw_queue_num_elems[cur_buffer] > 0) {
        RATR0_DEQUEUE_ARR(item, draw_queue, cur_buffer);
        queued_spec = &PIECE_SPECS[item.item.piece.piece].rotations[item.item.piece.rotation];
        draw_piece(backbuffer_surface, &tiles_surface,
                   &queued_spec->draw_spec,
                   item.item.piece.piece,
                   item.item.piece.row,
                   item.item.piece.col);

        // put this piece in the clear buffer for next time this
        // frame gets drawn
        if (item.item.piece.clear) {
            RATR0_ENQUEUE_ARR(clear_queue, cur_buffer, item);
        }
    }
}

/**
 * STATE FUNCTIONS
 */
struct CompletedRows completed_rows;
int done = 0;
void main_scene_update(struct Ratr0Scene *this_scene, UINT8 frames_elapsed);

/**
 * Move the specified rectangular region
 */
void _move_board_rect(struct Ratr0Surface *backbuffer_surface,
                      int from_row, int to_row, int num_rows)
{
    int srcx = BOARD_X0, srcy = BOARD_Y0 + from_row * 8,
        dstx = BOARD_X0, dsty = BOARD_Y0 + to_row * 8;
    int blit_width_pixels = BOARD_WIDTH * 8;
    int blit_height_pixels = num_rows * 8;

    // this is most likely overlapping, ratr0_blit_rect_simple()
    // will perform reverse copying if that is the case
    ratr0_blit_rect_simple(backbuffer_surface,
                           backbuffer_surface,
                           dstx, dsty,
                           srcx, srcy,
                           blit_width_pixels,
                           blit_height_pixels);
}

void process_move_queue()
{
    struct MoveQueueItem item;
    struct Ratr0Surface *backbuffer_surface = &ratr0_get_back_buffer()->surface;
    int cur_buffer = ratr0_get_back_buffer()->buffernum;
    while (move_queue_num_elems[cur_buffer] > 0) {
        RATR0_DEQUEUE_ARR(item, move_queue, cur_buffer);
        _move_board_rect(backbuffer_surface, item.from, item.to, item.num_rows);
    }
}


// This state shifts down the blocks that are left from deleting the lines
// Delete this function, it's only here for reference
int done_debug = 0;
void main_scene_debug(struct Ratr0Scene *this_scene,
                      UINT8 frame_elapsed) {
    struct Ratr0Surface *backbuffer_surface = &ratr0_get_back_buffer()->surface;
    int cur_buffer = ratr0_get_back_buffer()->buffernum;
    if (engine->input_system->was_action_pressed(action_quit)) {
        ratr0_engine_exit();
    }
    clear_rect(backbuffer_surface, 16, 0, 2, BOARD_WIDTH);
    clear_rect(backbuffer_surface, 17, 0, 2, BOARD_WIDTH);
    clear_rect(backbuffer_surface, 18, 0, 2, BOARD_WIDTH);
    if (done_debug < 4) {
        /*
        struct DrawQueueItem clear_row = {
            DRAW_TYPE_ROWS, { 0, 0 }
        };
        clear_row.item.rows.row = BOARD_HEIGHT - 2;
        clear_row.item.rows.num_rows = 1;
        RATR0_ENQUEUE_ARR(clear_queue, cur_buffer, clear_row);
        RATR0_ENQUEUE_ARR(clear_queue, ((cur_buffer + 1) % 2), clear_row);
        */
        done_debug++;
    }
    //process_blit_queues();
}


/**
 * The state to reorganize the board after lines were completed
 * That means collapsing all visible block to the bottom and
 * dropping all logical blocks down to the correct level
 */
void main_scene_reorganize_board(struct Ratr0Scene *this_scene,
                                 UINT8 frame_elapsed) {
    // For now, end when the mouse was clicked. This is just for testing
    if (engine->input_system->was_action_pressed(action_quit)) {
        ratr0_engine_exit();
    }
    process_blit_queues();
}

/**
 * This state cleans up the artifacts from establishing the dropped piece.
 * And then it deletes the lines that were marked
 * It should actually only last for 2 frames, to affect the 2 display
 * buffers.
 * Since we have 2 frames of CPU time where we don't do much, this is an
 * opportunity to calculate the update to the board.
 * The general rule is that we have to delete between 1 and 4 rows and
 * accordingly have to move a range of blocks that are on top of the
 * deleted rows:
 * - 1 or 4 deleted rows: 0 or 1 row regions to move
 * - 2 or 3 deleted rows: 0, 1 or 2 row regions to move
 * - if there are 3 deleted rows, there will be at least 2 rows that are
 *   together
 * - if there are 2 deleted rows, they are either together or 1 or 2 rows
 *   distance between
 */
BOOL done_delete_lines = 0;
void main_scene_delete_lines(struct Ratr0Scene *this_scene, UINT8 frames_elapsed)
{
    int cur_buffer = ratr0_get_back_buffer()->buffernum;

    // make sure this only gets executed once !!! Otherwise this will
    // keep queueing clear requests
    if (done_delete_lines == 0) {
        // 1. delete the lines: this could be combined with
        // some animation effect
        struct DrawQueueItem clear_row = {
            DRAW_TYPE_ROWS, { 0, 0 }
        };
        for (int i = 0; i < completed_rows.count; i++) {
            clear_row.item.rows.row = completed_rows.rows[i];
            RATR0_ENQUEUE_ARR(clear_queue, cur_buffer, clear_row);
            RATR0_ENQUEUE_ARR(clear_queue, ((cur_buffer + 1) % 2), clear_row);
        }
        // 2. move the regions above the deleted lines down graphically
        struct MoveRegions move_regions;
        get_move_regions(&move_regions, &completed_rows, &gameboard0);
        for (int i = 0; i < move_regions.count; i++) {
            struct MoveQueueItem move_item = {
                move_regions.regions[i].start,
                move_regions.regions[i].start + move_regions.regions[i].move_by,
                move_regions.regions[i].end - move_regions.regions[i].start
            };
            RATR0_ENQUEUE_ARR(move_queue, cur_buffer, move_item);
            RATR0_ENQUEUE_ARR(move_queue, ((cur_buffer + 1) % 2), move_item);
        }
        // 3. TODO: Delete the rows at the top, since moving won't get rid
        // of those
        // 4. TODO: compact the board logically by moving every block above
        // the deleted lines down
        delete_rows_from_board(&move_regions, &completed_rows, &gameboard0);
    }
    process_blit_queues();
    process_move_queue();
    done_delete_lines++;
    // switch to next state after 2 frames elapsed
    if (done_delete_lines >= 2) {
        this_scene->update = main_scene_reorganize_board;
    }
}

/**
 * A state that is simply there to establish the last game piece
 * and decide what to do from there. This is mainly to break up
 * the logic further into simple parts
 */
BOOL done_establish = FALSE;
void main_scene_establish_piece(struct Ratr0Scene *this_scene,
                                UINT8 frames_elapsed)
{
    int cur_buffer = ratr0_get_back_buffer()->buffernum;
    if (!done_establish) {
        // since we have a double buffer, we have to queue up a draw
        // for the following frame, too, but since this is an
        // etablished piece, don't clear it in the following frames
        struct DrawQueueItem dropped_item = (struct DrawQueueItem)
            {
                DRAW_TYPE_PIECE,
                {current_piece, current_rot, current_row, current_col, FALSE}
            };
        RATR0_ENQUEUE_ARR(draw_queue, cur_buffer, dropped_item);
        RATR0_ENQUEUE_ARR(draw_queue, ((cur_buffer + 1) % 2), dropped_item);
        establish_piece(current_piece, current_rot, current_row,
                        current_col, &gameboard0);
        done_establish = TRUE;
    } else {
        if (get_completed_rows(&completed_rows, current_piece,
                               current_rot, current_row,
                               &gameboard0)) {

            // delay the spawning, and delete completed lines first
            // switch state to deleting lines mode
            // but make sure, we update the drawing
            this_scene->update = main_scene_delete_lines;
        } else {
            spawn_next_piece(); // spawn next piece, but don't draw yet
            this_scene->update = main_scene_update;
        }
    }
    process_blit_queues();
}

/**
 * This is the main gameplay loop state. Handle input, rotate, move and place
 * blocks. The state is switched when there are completed lines
 */
void main_scene_update(struct Ratr0Scene *this_scene, UINT8 frames_elapsed)
{
    int cur_buffer = ratr0_get_back_buffer()->buffernum;

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
                DRAW_TYPE_PIECE,
                {
                    current_piece, current_rot, current_row, current_col, FALSE
                }
            };
            RATR0_ENQUEUE_ARR(clear_queue, cur_buffer, quick_drop_piece);

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

    // After input, was processed, we can determine what else is happening
    BOOL landed = FALSE;
    // automatic drop
    if (drop_timer == 0) {
        drop_timer = DROP_TIMER_VALUE;
        landed = piece_landed(current_piece, current_rot, current_row,
                              current_col, &gameboard0);
        if (landed) {
            done_establish = FALSE;
            this_scene->update = main_scene_establish_piece;
        } else {
            current_row++;
        }
    }

    // This is the default behavior if there was no establishment
    if (!landed) {
        // just draw the piece at the current position
        struct DrawQueueItem piece_to_draw = (struct DrawQueueItem)
            {
                DRAW_TYPE_PIECE,
                {
                    current_piece, current_rot, current_row, current_col, TRUE
                }
            };
        RATR0_ENQUEUE_ARR(draw_queue, cur_buffer, piece_to_draw);
        drop_timer--;
    }

    process_blit_queues();
    // Ghost piece is drawn with sprite, no background restore needed
    int qdr = get_quickdrop_row(current_piece, current_rot,
                                current_row, current_col,
                                &gameboard0);
    struct RotationSpec *rot_spec = &PIECE_SPECS[current_piece].rotations[current_rot];
    draw_ghost_piece(this_scene, &rot_spec->outline, qdr, current_col);
}

struct Ratr0Scene *setup_main_scene(Ratr0Engine *eng)
{
    debug_fp = fopen(DEBUG_FILE, "a");

    engine = eng;
    // Use the scenes module to create a scene and run that
    struct Ratr0NodeFactory *node_factory = engine->scenes_system->get_node_factory();
    struct Ratr0Scene *main_scene = node_factory->create_scene();
    //main_scene->update = main_scene_debug;
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

    // initialize board and  piece queue
    clear_board(&gameboard0);
    init_piece_queue();
    spawn_next_piece();
    return main_scene;
}
