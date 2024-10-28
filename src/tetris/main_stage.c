/** @file main_stage.c */
#include <stdio.h>
#include <string.h>

#include <ratr0/ratr0.h>
#include <ratr0/datastructs/queue.h>
#include <clib/graphics_protos.h>
#include <ratr0/hw_registers.h>
#include "main_stage.h"
#include "tetris_copper.h"
#include "game_data.h"
#include "draw_primitives.h"
#include "render_display.h"

struct Ratr0CopperListInfo TETRIS_COPPER_INFO = {
    3, 5, 7, 9,
    TETRIS_COPPER_BPLCON0_INDEX, TETRIS_COPPER_BPL1MOD_INDEX,
    TETRIS_COPPER_BPL1PTH_INDEX,
    TETRIS_COPPER_SPR0PTH_INDEX, TETRIS_COPPER_COLOR00_INDEX
};

// to debug the double buffer, we keep track of the current
// frame number. This will overflow and wrap to 0 at some
// point, but that's good enough
static UINT32 debug_current_frame = 0;

static Ratr0Engine *engine;
extern RATR0_ACTION_ID action_drop, action_move_left, action_move_right,
    action_move_down, action_rotate_right, action_rotate_left,
    action_hold,
    action_quit;

// Resources
#define BG_PATH_PAL ("tetris/assets/background_320x256x32.ts")
#define TILES_PATH  ("tetris/assets/tiles_32cols.ts")
#define DIGITS_PATH  ("tetris/assets/calculator_digits.ts")
#define DIGITS16_PATH  ("tetris/assets/calculator_digits_16.ts")
#define PREVIEW_PATH  ("tetris/assets/preview_pieces.ts")
#define OUTLINES_PATH  ("tetris/assets/block_outlines.spr")

// Sounds effects:
// we need rotate, drop and line deleted sounds
// TODO: maybe we should also have a sound for moving and for
// next level
#define SOUND_ROTATE_PATH ("tetris/assets/bb-bathit.raw8")
#define SOUND_DROP_PATH "tetris/assets/beep8bit.raw8"
#define SOUND_DELETELINES_PATH "tetris/assets/laser_zap.raw8"
#define SOUND_GAMEOVER_PATH "tetris/assets/sad_wah.raw8"

#define MUSIC_MAIN_PATH "tetris/soundtrack/onlyamiga.mod"
#define SOUNDFX_CHANNEL (3)

struct Ratr0TileSheet background_ts, tiles_ts, digits_ts,
    digits16_ts, preview_ts;
struct Ratr0Surface tiles_surface, digits_surface, digits16_surface,
    preview_surface;

// sound and music
struct Ratr0AudioSample drop_sound, rotate_sound, completed_sound,
    gameover_sound;
struct Ratr0AudioProtrackerMod main_music;

// ghost piece outline
struct Ratr0SpriteSheet outlines_sheet;
struct Ratr0HWSprite *outline_frame[9];

struct PlayerState player_state;

// game board, 0 means empty, if there is a piece it is block type + 1
// since the block types start at 0 as well
UINT8 gameboard0[BOARD_HEIGHT][BOARD_WIDTH];

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

void hide_ghost_piece(struct Ratr0Scene *scene)
{
    scene->sprites[0] = scene->sprites[1] = &NULL_HW_SPRITE;
}

struct PieceState current_piece = {
    PIECE_Z, 0, 0, 0
};

/** TIMERS THAT ARE CRITICAL TO GAME FEEL */
// rotation cooldown. We introduce a cooldown, to avoid the player
// piece rotating way too fast
#define ROTATE_COOLDOWN_TIME (10)
int rotate_cooldown = 0;
int drop_timer = DROP_TIMER_VALUE;
#define QUICKDROP_COOLDOWN_TIME (10)
int quickdrop_cooldown = 0;
// TODO: this should be dependent on frame rate, so 6 for NTSC and 5 for
// PAL
#define MOVE_COOLDOWN_TIME (5)
int move_cooldown = 0;

void spawn_next_piece(void)
{
    current_piece.row = 0;
    current_piece.col = 4;  // center the piece
    current_piece.rotation = 0;
    current_piece.piece = piece_queue[piece_queue_idx++];
    piece_queue_idx %= PIECE_QUEUE_LEN;
}

/**
 * STATE FUNCTIONS
 */
struct CompletedRows completed_rows;
int done = 0;
void main_scene_update(struct Ratr0Scene *this_scene,
                       struct Ratr0DisplayBuffer *backbuffer,
                       UINT8 frames_elapsed);


BOOL done_gameover_once = FALSE;
void main_scene_gameover(struct Ratr0Scene *this_scene,
                         struct Ratr0DisplayBuffer *backbuffer,
                         UINT8 frame_elapsed) {
    if (engine->input_system->was_action_pressed(action_quit)) {
        ratr0_engine_exit();
    }

    if (!done_gameover_once) {
        // 1. establish the piece in the board by enqueueing the dropped
        // piece, this makes sure it does not blink
        int cur_buffer = backbuffer->buffernum;
        struct PieceQueueItem dropped_item = (struct PieceQueueItem) {
            current_piece.piece, current_piece.rotation,
            current_piece.row, current_piece.col, FALSE
        };
        RATR0_ENQUEUE_ARR(draw_piece_queue, cur_buffer, dropped_item);
        RATR0_ENQUEUE_ARR(draw_piece_queue, ((cur_buffer + 1) % 2), dropped_item);

        // 2. play game over sound
        ratr0_audio_play_sound(&gameover_sound, SOUNDFX_CHANNEL);

        // 3. stop music
        ratr0_audio_stop_mod();

        // 4. TODO: display the game over message.

        done_gameover_once = TRUE;
    }

    process_blit_queues(backbuffer, &tiles_surface, &preview_surface,
                        &digits16_surface);
}

/**
 * DEBUG STATE. Use this state to quickly simulate game situations
 * that would otherwise require playing to this state.
 */
void main_scene_debug(struct Ratr0Scene *this_scene,
                      struct Ratr0DisplayBuffer *backbuffer,
                      UINT8 frame_elapsed) {
    if (engine->input_system->was_action_pressed(action_quit)) {
        ratr0_engine_exit();
    }
    draw_level_digit(backbuffer, &digits16_surface, 0, '2');
    draw_lines_digit(backbuffer, &digits16_surface, 0, '3');
}


void _update_level(void)
{
    struct DigitQueueItem digit = {player_state.difficulty_level, 0};
    RATR0_ENQUEUE_ARR(level_queue, 0, digit);
    RATR0_ENQUEUE_ARR(level_queue, 1, digit);
}

void _update_lines(void)
{
    struct DigitQueueItem digit = {player_state.level_cleared_rows, 0};
    RATR0_ENQUEUE_ARR(lines_queue, 0, digit);
    RATR0_ENQUEUE_ARR(lines_queue, 1, digit);
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
 *
 * ENHANCEMENT (OPTIONAL)
 * We could optional delete the lines: this could be combined with
 * some animation effect. Note that because our rendering is queue-based
 * we would need to do it in a a different state function, otherwise
 * our queue processing ordering will interfere
 */
BOOL done_delete_lines = 0;
void main_scene_delete_lines(struct Ratr0Scene *this_scene,
                             struct Ratr0DisplayBuffer *backbuffer,
                             UINT8 frames_elapsed)
{
    int cur_buffer = backbuffer->buffernum;

    // make sure this only gets executed once !!! Otherwise this will
    // keep queueing clear requests
    if (done_delete_lines == 0) {
        struct RowQueueItem clear_row = { 0, 0};
#ifdef DEBUG_TETRIS
        fprintf(debug_fp, "# completed rows: %u\n", completed_rows.count);
#endif
        // add score for deleted lines
        BOOL level_increased = score_rows_cleared(&player_state,
                                                  completed_rows.count);
        if (level_increased) {
            // TODO: update level and drop speed display in the UI
            // by adding commands to the queue
            _update_level();
        }
        _update_lines();

        // play completed sound
        ratr0_audio_play_sound(&completed_sound, SOUNDFX_CHANNEL);
        // 1. move the regions above the deleted lines down graphically
        struct MoveRegions move_regions;
        get_move_regions(&move_regions, &completed_rows, &gameboard0);
#ifdef DEBUG_TETRIS
        fprintf(debug_fp, "# move regions: %u\n", move_regions.count);
#endif
        for (int i = 0; i < move_regions.count; i++) {
            struct MoveQueueItem move_item = {
                move_regions.regions[i].start,
                move_regions.regions[i].start + move_regions.regions[i].move_by,
                move_regions.regions[i].end - move_regions.regions[i].start + 1
            };
#ifdef DEBUG_TETRIS
            fprintf(debug_fp, "# move_region #%i: start: %u end: %u by: %u\n",
                    i, move_regions.regions[i].start,
                    move_regions.regions[i].end, move_regions.regions[i].move_by);
#endif
            RATR0_ENQUEUE_ARR(move_queue, cur_buffer, move_item);
            RATR0_ENQUEUE_ARR(move_queue, ((cur_buffer + 1) % 2), move_item);
        }
        // 2. Delete the rows at the top, since moving won't get rid
        // of those
        int move_top = move_regions.regions[move_regions.count - 1].start;
        clear_row.row = move_top;
        clear_row.num_rows = completed_rows.count;

        // debug output
#ifdef DEBUG_TETRIS
        fprintf(debug_fp, "clearing %u rows off the top starting from: %d\n",
                completed_rows.count, move_top);
#endif
        RATR0_ENQUEUE_ARR(clear_row_queue, cur_buffer, clear_row);
        RATR0_ENQUEUE_ARR(clear_row_queue, ((cur_buffer + 1) % 2), clear_row);

        // 3. compact the board logically by moving every block above
        // the deleted lines down
#ifdef DEBUG_TETRIS
        fprintf(debug_fp, "BOARD BEFORE compacting\n");
        dump_board(debug_fp, &gameboard0);
#endif
        delete_rows_from_board(&move_regions, &completed_rows, &gameboard0);
#ifdef DEBUG_TETRIS
        fprintf(debug_fp, "BOARD AFTER compacting\n");
        dump_board(debug_fp, &gameboard0);
#endif
    }
    // move must come before blit, because the blit queues will delete rows !!!
    process_move_queue(backbuffer);
    process_blit_queues(backbuffer, &tiles_surface, &preview_surface,
                        &digits16_surface);
    done_delete_lines++;
    // switch to next state after 2 frames elapsed
    if (done_delete_lines >= 2) {
        spawn_next_piece(); // spawn next piece, but don't draw yet
        enqueue_next3();
        done_delete_lines = 0; // reset frame counter for the next time
        player_state.can_swap_hold = TRUE;
        // back to main update state
        this_scene->update = main_scene_update;
    }
    debug_current_frame++;
}

/**
 * A state that is simply there to establish the last game piece
 * and decide what to do from there. This is mainly to break up
 * the logic further into simple parts
 */
BOOL done_establish = FALSE;
void main_scene_establish_piece(struct Ratr0Scene *this_scene,
                                struct Ratr0DisplayBuffer *backbuffer,
                                UINT8 frames_elapsed)
{
    int cur_buffer = backbuffer->buffernum;
    if (!done_establish) {
        ratr0_audio_play_sound(&drop_sound, SOUNDFX_CHANNEL);
        // since we have a double buffer, we have to queue up a draw
        // for the following frame, too, but since this is an
        // etablished piece, don't clear it in the following frames
        struct PieceQueueItem dropped_item = (struct PieceQueueItem) {
            current_piece.piece, current_piece.rotation,
            current_piece.row, current_piece.col, FALSE
        };
        RATR0_ENQUEUE_ARR(draw_piece_queue, cur_buffer, dropped_item);
        RATR0_ENQUEUE_ARR(draw_piece_queue, ((cur_buffer + 1) % 2), dropped_item);
        establish_piece(&current_piece, &gameboard0);

        done_establish = TRUE;
    } else {
        UINT8 num_completed_rows = get_completed_rows(&completed_rows,
                                                      &current_piece,
                                                      &gameboard0);
        if (num_completed_rows > 0) {
            // delay the spawning, and delete completed lines first
            // switch state to deleting lines mode
            // but make sure, we update the drawing
            this_scene->update = main_scene_delete_lines;
        } else {
            spawn_next_piece(); // spawn next piece, but don't draw yet
            enqueue_next3();
            player_state.can_swap_hold = TRUE;
            this_scene->update = main_scene_update;
        }
    }
    process_blit_queues(backbuffer, &tiles_surface, &preview_surface,
                        &digits16_surface);
    debug_current_frame++;
}

/**
 * This is the main gameplay loop state. Handle input, rotate, move and place
 * blocks. The state is switched when there are completed lines
 */
void main_scene_update(struct Ratr0Scene *this_scene,
                       struct Ratr0DisplayBuffer *backbuffer,
                       UINT8 frames_elapsed)
{
    int cur_buffer = backbuffer->buffernum;

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
        if (move_cooldown == 0 && can_move_left(&current_piece,
                                                &gameboard0)) {
            current_piece.col--;
            move_cooldown = MOVE_COOLDOWN_TIME;
        }
    } else if (engine->input_system->was_action_pressed(action_move_right)) {
        if (move_cooldown == 0 && can_move_right(&current_piece,
                                                 &gameboard0)) {
            current_piece.col++;
            move_cooldown = MOVE_COOLDOWN_TIME;
        }
    } else if (engine->input_system->was_action_pressed(action_move_down)) {
        // ACCELERATE MOVE DOWN
        if (!piece_landed(&current_piece, &gameboard0)) {
            score_soft_drop(&player_state);
            current_piece.row++;
            drop_timer = player_state.drop_timer_value; // reset drop timer

            // TODO: update score panel
        }
    } else if (engine->input_system->was_action_pressed(action_drop)) {
        // QUICK DROP
        // do a quick establish on the quickdrop row
        // we can do this by setting current_row to qdr and the drop timer
        // to 0
        if (quickdrop_cooldown == 0) {
            // we need to enqueue the current position into a clear queue
            // so we delete the graphics of the piece
            struct PieceQueueItem clear_quickdrop_piece = {
                current_piece.piece, current_piece.rotation,
                current_piece.row, current_piece.col, FALSE
            };
            RATR0_ENQUEUE_ARR(clear_piece_queue, cur_buffer,
                              clear_quickdrop_piece);

            // now we update to the drop/establish condition
            int old_row = current_piece.row;
            current_piece.row = get_quickdrop_row(&current_piece,
                                                  &gameboard0);
            score_hard_drop(&player_state, current_piece.row - old_row);

            // this means we drop immediately
            drop_timer = 0;
            quickdrop_cooldown = QUICKDROP_COOLDOWN_TIME;
            // TODO: update score panel
        }
    } else if (engine->input_system->was_action_pressed(action_rotate_right)) {
        // rotate right with wall kick
        if (rotate_cooldown == 0) {
            int next_rotation = (current_piece.rotation + 1) % 4;
            struct Translate *t = get_srs_translation(&current_piece,
                                                      next_rotation,
                                                      &gameboard0);
            if (t) {
                current_piece.rotation = next_rotation;
                // SRS wall kick data actually denotes y in reverse
                // so we need to subtract y
                current_piece.row -= t->y;
                current_piece.col += t->x;
                ratr0_audio_play_sound(&rotate_sound, SOUNDFX_CHANNEL);
            }
            rotate_cooldown = ROTATE_COOLDOWN_TIME;
        }
    } else if (engine->input_system->was_action_pressed(action_rotate_left)) {
        // rotate left with wall kick
        if (rotate_cooldown == 0) {
            int next_rotation = (current_piece.rotation - 1) % 4;
            struct Translate *t = get_srs_translation(&current_piece,
                                                      next_rotation,
                                                      &gameboard0);
            if (t) {
                current_piece.rotation = next_rotation;
                // SRS wall kick data actually denotes y in reverse
                // so we need to subtract y
                current_piece.row -= t->y;
                current_piece.col += t->x;
            }
            rotate_cooldown = ROTATE_COOLDOWN_TIME;
        }
    } else if (engine->input_system->was_action_pressed(action_hold)) {
        if (player_state.can_swap_hold) {
            if (player_state.hold == -1) {
                // just move the current piece to the hold and
                // and spawn next piece
                player_state.hold = current_piece.piece;
                spawn_next_piece();
                enqueue_next3();
            } else {
                // swap the hold pieces and reset the position
                INT8 tmp = player_state.hold;
                player_state.hold = current_piece.piece;
                current_piece.piece = tmp;
                current_piece.col = 4;
                current_piece.row = 0;
                current_piece.rotation = 0;

            }

            // enqueue hold piece
            struct HoldQueueItem hold_item;
            hold_item.piece = player_state.hold;
            RATR0_ENQUEUE_ARR(hold_queue, 0, hold_item);
            RATR0_ENQUEUE_ARR(hold_queue, 1, hold_item);

            player_state.can_swap_hold = FALSE;
        }
    }

    // After input, was processed, we can determine what else is happening
    BOOL landed = FALSE;
    // automatic drop
    if (drop_timer == 0) {
        drop_timer = player_state.drop_timer_value;
        landed = piece_landed(&current_piece, &gameboard0);
        if (landed) {
            done_establish = FALSE;
            this_scene->update = (current_piece.row == 0) ?
                this_scene->update = main_scene_gameover :
                main_scene_establish_piece;
        } else {
            current_piece.row++;
        }
    }

    // This is the default behavior if there was no establishment
    if (!landed) {
        // just draw the piece at the current position
        struct PieceQueueItem piece_to_draw = (struct PieceQueueItem) {
            current_piece.piece, current_piece.rotation,
            current_piece.row, current_piece.col, TRUE
        };
        RATR0_ENQUEUE_ARR(draw_piece_queue, cur_buffer, piece_to_draw);
        drop_timer--;
    }
    process_blit_queues(backbuffer, &tiles_surface, &preview_surface,
                        &digits16_surface);
    if (landed) {
        hide_ghost_piece(this_scene);
    } else {
        // Ghost piece is drawn with sprite, no background restore needed
        int qdr = get_quickdrop_row(&current_piece,
                                    &gameboard0);
        struct RotationSpec *rot_spec = &PIECE_SPECS[current_piece.piece].rotations[current_piece.rotation];
        draw_ghost_piece(this_scene, &rot_spec->outline, qdr, current_piece.col);
    }
    debug_current_frame++;
}

static void _load_resources(void)
{
    // Load background
    struct Ratr0Surface bg_surf;
    BOOL ts_read = ratr0_resources_read_tilesheet(BG_PATH_PAL, &background_ts);
    ratr0_resources_init_surface_from_tilesheet(&bg_surf, &background_ts);
    ratr0_display_blit_surface_to_buffers(&bg_surf);
    ratr0_display_set_palette(background_ts.palette, 32, 0);

    // from here we don't need to the memory for the background
    // anymore, we can free the surface and tilesheet
    ratr0_resources_free_tilesheet_data(&background_ts);

    // Load tileset for the blocks
    ts_read = ratr0_resources_read_tilesheet(TILES_PATH, &tiles_ts);
    ratr0_resources_init_surface_from_tilesheet(&tiles_surface, &tiles_ts);

    // load block outlines as sprite for the ghost piece
    ratr0_resources_read_spritesheet(OUTLINES_PATH, &outlines_sheet);
    for (int i = 0; i < outlines_sheet.header.num_sprites; i++) {
        outline_frame[i] = ratr0_create_sprite_from_sprite_sheet_frame(&outlines_sheet, i);
    }
    // load digit tile set for the score panels
    ts_read = ratr0_resources_read_tilesheet(DIGITS_PATH, &digits_ts);
    ratr0_resources_init_surface_from_tilesheet(&digits_surface, &digits_ts);

    ts_read = ratr0_resources_read_tilesheet(DIGITS16_PATH, &digits16_ts);
    ratr0_resources_init_surface_from_tilesheet(&digits16_surface, &digits16_ts);

    // load preview piece tile set
    ts_read = ratr0_resources_read_tilesheet(PREVIEW_PATH, &preview_ts);
    if (!ts_read) {
        fprintf(debug_fp, "could not read preview tiles !!!\n");
    }
    ratr0_resources_init_surface_from_tilesheet(&preview_surface, &preview_ts);

    // read sound effects and music
    ratr0_resources_read_audiosample(SOUND_DROP_PATH, &drop_sound);
    ratr0_resources_read_audiosample(SOUND_ROTATE_PATH, &rotate_sound);
    ratr0_resources_read_audiosample(SOUND_DELETELINES_PATH, &completed_sound);
    ratr0_resources_read_audiosample(SOUND_GAMEOVER_PATH, &gameover_sound);

    BOOL ret = ratr0_resources_read_protracker(MUSIC_MAIN_PATH, &main_music);
    if (!ret) {
        fprintf(debug_fp, "could not read protracker module '%s'\n",
                MUSIC_MAIN_PATH);
    }
}

void main_stage_on_enter(struct Ratr0Scene *this_scene)
{
    // set new copper list
    ratr0_display_init_copper_list(tetris_copper, TETRIS_COPPER_SIZE_WORDS,
                                   &TETRIS_COPPER_INFO);

    ratr0_display_set_copperlist(tetris_copper, TETRIS_COPPER_SIZE_WORDS,
                                 &TETRIS_COPPER_INFO);

    _load_resources();

    // initialize board and  piece queue
    clear_board(&gameboard0);
    reset_player_state(&player_state);
    init_piece_queue(&piece_queue);
    piece_queue_idx = 0;
    spawn_next_piece();
    enqueue_next3();
    _update_level();
    _update_lines();
    player_state.can_swap_hold = TRUE;
    done_gameover_once = FALSE;

    // start bg music
    ratr0_audio_play_mod(&main_music);
}

void main_stage_on_exit(struct Ratr0Scene *this_scene)
{
    // free all memory from the assets in reverse order
    // to make sure it all becomes available again
    // note: we could have RATR0 have a deallocation marker
    // so the memory could be freed all at once
    ratr0_resources_free_protracker_data(&main_music);
    ratr0_resources_free_audiosample_data(&gameover_sound);
    ratr0_resources_free_audiosample_data(&completed_sound);
    ratr0_resources_free_audiosample_data(&rotate_sound);
    ratr0_resources_free_audiosample_data(&drop_sound);

    ratr0_resources_free_tilesheet_data(&preview_ts);
    ratr0_resources_free_tilesheet_data(&digits16_ts);
    ratr0_resources_free_tilesheet_data(&digits_ts);

    ratr0_resources_free_spritesheet_data(&outlines_sheet);
    ratr0_resources_free_tilesheet_data(&tiles_ts);

    // clear all rendering related queues
    clear_render_queues();
}

struct Ratr0Scene *setup_main_scene(Ratr0Engine *eng)
{
    engine = eng;
    // Use the scenes module to create a scene and run that
    struct Ratr0NodeFactory *node_factory = engine->scenes_system->get_node_factory();
    struct Ratr0Scene *main_scene = node_factory->create_scene();
    //main_scene->update = main_scene_debug;
    main_scene->update = main_scene_update;
    main_scene->on_enter = main_stage_on_enter;
    main_scene->on_exit = main_stage_on_exit;

    return main_scene;
}
