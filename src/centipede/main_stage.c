/** @file main_stage.c */
#include <stdio.h>
#include <string.h>

#include <ratr0/ratr0.h>
#include <clib/graphics_protos.h>
#include "main_stage.h"

static Ratr0Engine *engine;
extern RATR0_ACTION_ID action_fire, action_move_left, action_move_right;

// Resources
#define GRID_PATH ("sinvaders/assets/basegrid_288x256x2.ts")
#define CENTI_HEAD_LR_PATH ("centipede/assets/c_head_l2r.ts")
struct Ratr0TileSheet grid_sheet, centi_head_lr_sheet;
struct Ratr0SpriteSheet sprite_sheet;
UINT8 centi_head_lr_frames[] = {0, 1, 2, 3, 4};

void main_scene_update(struct Ratr0Scene *this_scene, UINT8 frames_elapsed)
{
    // For now, end when the mouse was clicked. This is just for testing
    if (engine->input_system->was_action_pressed(action_fire)) {
        ratr0_engine_exit();
    }
    if (engine->input_system->was_action_pressed(action_move_left)) {
        //
    } else if (engine->input_system->was_action_pressed(action_move_right)) {
        //
    }
}


struct Ratr0Scene *setup_main_scene(Ratr0Engine *eng)
{
    engine = eng;
    // Use the scenes module to create a scene and run that
    struct Ratr0NodeFactory *node_factory = engine->scenes_system->get_node_factory();
    struct Ratr0Scene *main_scene = node_factory->create_scene();
    main_scene->update = main_scene_update;

    engine->resource_system->read_tilesheet(GRID_PATH, &grid_sheet);
    ratr0_display_set_palette(grid_sheet.palette, 4, 0);

    // can we do this in one step ?
    engine->resource_system->read_tilesheet(CENTI_HEAD_LR_PATH, &centi_head_lr_sheet);
    struct Ratr0AnimatedHWSprite *anim_sprite = (struct Ratr0AnimatedHWSprite *)
        node_factory->create_sprite(&centi_head_lr_sheet,
                                    centi_head_lr_frames, 5, 1, TRUE);
    main_scene->set_sprite_at(main_scene, anim_sprite, 0);

    anim_sprite->base_obj.bounds.x = 0;
    anim_sprite->base_obj.bounds.y = 10;

    struct Ratr0Surface *back_buffer, *front_buffer;
    front_buffer = ratr0_get_front_buffer();
    back_buffer = ratr0_get_back_buffer();
    return main_scene;
}
