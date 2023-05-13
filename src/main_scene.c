/** @file main_scene.c */
#include <ratr0/engine.h>
#include <ratr0/amiga/engine.h>
#include <ratr0/input.h>
#include <ratr0/main_scene.h>

static Ratr0Engine *engine;
extern RATR0_ACTION_ID action_fire, action_move_left, action_move_right;
struct Ratr0AnimatedAmigaBob *bobs[2];

void main_scene_update(struct Ratr0Scene *this_scene, UINT8 frames_elapsed)
{
    // TODO: Animate BOBs
    // For now, end when the mouse was clicked. This is just for testing
    if (engine->input_system->was_action_pressed(action_fire)) {
        ratr0_amiga_engine_exit();
    }
    if (engine->input_system->was_action_pressed(action_move_left)) {
        bobs[0]->base_obj.translate.x--;
    } else if (engine->input_system->was_action_pressed(action_move_right)) {
        bobs[0]->base_obj.translate.x++;
    }
}

// Resources
#define BOBS_PATH_IL ("test_assets/fox_run_20x23x3.ts")
#define GRID_PATH ("test_assets/basegrid_320x256x3.ts")
struct Ratr0TileSheet bobs_il, grid;
UINT8 bob_frames[6] = {0, 1, 2, 3, 4, 5};
extern struct Ratr0Backdrop *backdrop;  // GLOBAL for performance testing

struct Ratr0Scene *setup_main_scene(Ratr0Engine *eng)
{
    engine = eng;
    // Use the scenes module to create a scene and run that
    struct Ratr0NodeFactory *node_factory = engine->scenes_system->get_node_factory();
    struct Ratr0Scene *main_scene = node_factory->create_scene();
    main_scene->update = main_scene_update;

    engine->resource_system->read_tilesheet(GRID_PATH, &grid);
    ratr0_amiga_set_palette(grid.palette, 8);

    engine->resource_system->read_tilesheet(BOBS_PATH_IL, &bobs_il);
    bobs[0] = (struct Ratr0AnimatedAmigaBob *) node_factory->create_sprite(&bobs_il, bob_frames, 6, 5, FALSE);
    bobs[0]->base_obj.bounds.x = 50;
    bobs[0]->base_obj.bounds.y = 16;

    bobs[1] = (struct Ratr0AnimatedAmigaBob *) node_factory->create_sprite(&bobs_il, bob_frames, 6, 10, FALSE);
    bobs[1]->base_obj.bounds.x = 83;
    bobs[1]->base_obj.bounds.y = 32;

    bobs[0]->base_obj.node.next = (struct Ratr0Node *) bobs[1];
    main_scene->bobs = bobs[0];
    main_scene->backdrop = node_factory->create_backdrop(&grid);
    return main_scene;
}
