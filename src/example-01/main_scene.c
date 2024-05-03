/** @file main_scene.c */
#include <ratr0/engine.h>
#include <ratr0/input.h>
#include "main_scene.h"

// Resources
#define BOBS_PATH_IL ("test_assets/fox_run_20x23x3.ts")
#define GRID_PATH ("test_assets/basegrid_320x256x3.ts")
#define NUM_BOBS (4)

struct Ratr0TileSheet bobs_il, grid;

UINT8 bob_frames[6] = {0, 1, 2, 3, 4, 5};

extern struct Ratr0Backdrop *backdrop;  // GLOBAL for performance testing

static Ratr0Engine *engine;
extern RATR0_ACTION_ID action_fire, action_move_left, action_move_right, action_exit;
struct Ratr0AnimatedAmigaBob *bobs[NUM_BOBS];

void main_scene_update(struct Ratr0Scene *this_scene, UINT8 frames_elapsed)
{
    // TODO: Animate BOBs
    // For now, end when the mouse was clicked. This is just for testing
    if (engine->input_system->was_action_pressed(action_fire)) {
        ratr0_engine_exit();
    }
    if (engine->input_system->was_action_pressed(action_move_left)) {
        bobs[0]->base_obj.translate.x--;
    } else if (engine->input_system->was_action_pressed(action_move_right)) {
        bobs[0]->base_obj.translate.x++;
    }
    if (engine->input_system->was_action_pressed(action_exit)) {
        ratr0_engine_exit();
    }
}


struct Ratr0Scene *setup_main_scene(Ratr0Engine *eng)
{
    engine = eng;
    // Use the scenes module to create a scene and run that
    struct Ratr0NodeFactory *node_factory = engine->scenes_system->get_node_factory();
    struct Ratr0Scene *main_scene = node_factory->create_scene();
    main_scene->update = main_scene_update;

    engine->resource_system->read_tilesheet(GRID_PATH, &grid);
    ratr0_amiga_set_palette(grid.palette, 8, 0);

    engine->resource_system->read_tilesheet(BOBS_PATH_IL, &bobs_il);
    bobs[0] = (struct Ratr0AnimatedAmigaBob *) node_factory->create_sprite(&bobs_il, bob_frames, 6, 5, FALSE);
    bobs[0]->base_obj.bounds.x = 50;
    bobs[0]->base_obj.bounds.y = 16;

    bobs[1] = (struct Ratr0AnimatedAmigaBob *) node_factory->create_sprite(&bobs_il, bob_frames, 6, 10, FALSE);
    bobs[1]->base_obj.bounds.x = 83;
    bobs[1]->base_obj.bounds.y = 32;

    bobs[2] = (struct Ratr0AnimatedAmigaBob *) node_factory->create_sprite(&bobs_il, bob_frames, 6, 10, FALSE);
    bobs[2]->base_obj.bounds.x = 120;
    bobs[2]->base_obj.bounds.y = 50;

    bobs[3] = (struct Ratr0AnimatedAmigaBob *) node_factory->create_sprite(&bobs_il, bob_frames, 6, 10, FALSE);
    bobs[3]->base_obj.bounds.x = 140;
    bobs[3]->base_obj.bounds.y = 63;

    for (int i = 0; i < NUM_BOBS; i++) {
        main_scene->bobs[i] = bobs[i];
    }
    main_scene->num_bobs = NUM_BOBS;
    main_scene->backdrop = node_factory->create_backdrop(&grid);
    return main_scene;
}
