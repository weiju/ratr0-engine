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
    action_quit;

// Resources
#define BG_PATH_PAL ("tetris/assets/background_320x256x32.ts")
#define TILES_PATH  ("tetris/assets/tiles_32cols.ts")

struct Ratr0TileSheet background_ts, tiles_ts;
struct Ratr0Surface *backbuffer_surface, tiles_surface;

static int diag_count = 0;

void main_scene_update(struct Ratr0Scene *this_scene, UINT8 frames_elapsed)
{
    // For now, end when the mouse was clicked. This is just for testing
    if (engine->input_system->was_action_pressed(action_quit)) {
        ratr0_engine_exit();
    }
    if (engine->input_system->was_action_pressed(action_move_left)) {
        //
    } else if (engine->input_system->was_action_pressed(action_move_right)) {
        //
    } else if (engine->input_system->was_action_pressed(action_drop)) {
    }

    // Test blitting a tile
    backbuffer_surface = ratr0_get_back_buffer();
    /*
    ratr0_blit_rect_simple(backbuffer_surface, &tiles_surface, 48, 16, 0, 0, 16, 8);
    ratr0_blit_rect_simple(backbuffer_surface, &tiles_surface, 48, 24, 0, 0, 16, 8);
    // Shift by 8
    ratr0_blit_rect(backbuffer_surface, &tiles_surface, 56, 32, 0, 0, 16, 8);
    // on a middle row tile
    ratr0_blit_rect(backbuffer_surface, &tiles_surface, 56, 40, 0, 64, 16, 8);
    */
    //ratr0_blit_rect(backbuffer_surface, &tiles_surface, 48, 16, 0, 0, 8, 8);
    //ratr0_blit_rect(backbuffer_surface, &tiles_surface, 48, 24, 0, 0, 24, 8);
    if (diag_count == 0) {
        ratr0_diag_blit_rect_ad(backbuffer_surface, &tiles_surface, 56, 16, 0, 0, 24, 8);
        diag_count++;
    } else {
        ratr0_blit_rect_ad(backbuffer_surface, &tiles_surface, 56, 16, 0, 0, 24, 8);
    }
    //ratr0_blit_rect(backbuffer_surface, &tiles_surface, 56, 24, 0, 0, 24, 8);
    // on a middle row tile
    // Test the J Blit data
    // 48,16 is the left top corner of the game board
    // BUG: We currently can't blit rects that are not a multiple of 16
    // which means, alwm and afwm are not set correctly
    /*
    int dsty = 16;
    for (int i = 0; i < J_SPEC.blit_specs[0].num_blits; i++) {
        ratr0_blit_rect(backbuffer_surface, &tiles_surface,
                        48, dsty,
                        J_SPEC.blit_specs[0].blit_rects[i].srcx,
                        J_SPEC.blit_specs[0].blit_rects[i].srcy,
                        J_SPEC.blit_specs[0].blit_rects[i].width,
                        J_SPEC.blit_specs[0].blit_rects[i].height);
        dsty += 16;
        }
    */
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

    struct Position *jr0 = J_SPEC.rotations[0];
    int jr0_numblits = J_SPEC.blit_specs[0].num_blits;
    printf("JR0, # blits: %d, [(%d, %d), (%d, %d), (%d, %d), (%d, %d)]",
           jr0_numblits, jr0[0].x, jr0[0].y, jr0[1].x, jr0[1].y,
           jr0[2].x, jr0[2].y, jr0[3].x, jr0[3].y);

    return main_scene;
}
