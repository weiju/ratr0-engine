/** @file main_scene.c */
#include <ratr0/ratr0.h>
#include "main_scene.h"

// Resources
#define BOBS_PATH_IL ("assets/fox_run_20x23x3.ts")
#define GRID_PATH ("assets/basegrid_320x256x3.ts")
#define SPRITES_PATH ("assets/fox-run_00.spr")
#define NUM_BOBS (4)

struct Ratr0TileSheet bobs_il, grid;

UINT8 bob_frames[6] = {0, 1, 2, 3, 4, 5};

extern struct Ratr0Backdrop *backdrop;  // GLOBAL for performance testing

static Ratr0Engine *engine;
extern RATR0_ACTION_ID action_fire, action_move_left, action_move_right, action_exit;
struct Ratr0Bob *bobs[NUM_BOBS];

struct Ratr0SpriteSheet fox_sprite_sheet;
UINT8 fox_frames[] = {0, 1, 2, 3, 4};
struct Ratr0HWSprite *fox;

void main_stage_update(struct Ratr0Stage *this_stage,
                       struct Ratr0DisplayBuffer *backbuffer,
                       UINT8 frames_elapsed)
{
    // TODO: Animate BOBs
    // For now, end when the mouse was clicked. This is just for testing
    if (ratr0_input_was_action_pressed(action_fire)) {
        ratr0_engine_exit();
    }
    if (ratr0_input_was_action_pressed(action_move_left)) {
        bobs[0]->base_obj.translate.x--;
    } else if (ratr0_input_was_action_pressed(action_move_right)) {
        bobs[0]->base_obj.translate.x++;
    }
    if (ratr0_input_was_action_pressed(action_exit)) {
        ratr0_engine_exit();
    }
}


struct Ratr0Stage *setup_main_stage(Ratr0Engine *eng)
{
    engine = eng;
    // Use the stages module to create a stage and run that
    struct Ratr0NodeFactory *node_factory = ratr0_stages_get_node_factory();
    struct Ratr0Stage *main_stage = node_factory->create_stage();
    main_stage->update = main_stage_update;

    ratr0_resources_read_tilesheet(GRID_PATH, &grid);
    ratr0_display_set_palette(grid.palette, 8, 0);

    ratr0_resources_read_tilesheet(BOBS_PATH_IL, &bobs_il);
    bobs[0] = (struct Ratr0Bob *) node_factory->create_sprite(&bobs_il, bob_frames, 6, 5, FALSE);
    bobs[0]->base_obj.bounds.x = 50;
    bobs[0]->base_obj.bounds.y = 16;

    bobs[1] = (struct Ratr0Bob *) node_factory->create_sprite(&bobs_il, bob_frames, 6, 10, FALSE);
    bobs[1]->base_obj.bounds.x = 83;
    bobs[1]->base_obj.bounds.y = 32;

    bobs[2] = (struct Ratr0Bob *) node_factory->create_sprite(&bobs_il, bob_frames, 6, 10, FALSE);
    bobs[2]->base_obj.bounds.x = 120;
    bobs[2]->base_obj.bounds.y = 50;

    bobs[3] = (struct Ratr0Bob *) node_factory->create_sprite(&bobs_il, bob_frames, 6, 10, FALSE);
    bobs[3]->base_obj.bounds.x = 140;
    bobs[3]->base_obj.bounds.y = 63;

    for (int i = 0; i < NUM_BOBS; i++) {
        main_stage->bobs[i] = bobs[i];
    }
    main_stage->num_bobs = NUM_BOBS;
    main_stage->backdrop = node_factory->create_backdrop(&grid);

    // 1. Read animated sprites from sprite sheet
    ratr0_resources_read_spritesheet(SPRITES_PATH, &fox_sprite_sheet);
    fox = ratr0_create_sprite_from_sprite_sheet(&fox_sprite_sheet, (UINT8) 6, RATR0_ANIM_LOOP_TYPE_NONE);
    main_stage->sprites[main_stage->num_sprites++] = fox;
    fox->base_obj.bounds.x = 0;
    fox->base_obj.bounds.y = 40;

    return main_stage;
}
