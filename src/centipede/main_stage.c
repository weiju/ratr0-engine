/** @file main_stage.c */
#include <stdio.h>
#include <string.h>

#include <ratr0/ratr0.h>
#include <clib/graphics_protos.h>
#include <ratr0/hw_registers.h>
#include "main_stage.h"
#include "centipede_copper.h"

struct Ratr0CopperListInfo CENTI_COPPER_INFO = {
    3, 5, 7, 9,
    CENTI_COPPER_BPLCON0_INDEX, CENTI_COPPER_BPL1MOD_INDEX,
    CENTI_COPPER_BPL1PTH_INDEX,
    CENTI_COPPER_SPR0PTH_INDEX, CENTI_COPPER_COLOR00_INDEX
};

static Ratr0Engine *engine;
extern RATR0_ACTION_ID action_fire, action_move_left, action_move_right;

// Resources
#define CENTI_HEAD_LR_PATH ("centipede/assets/c_head_l2r.spr")
struct Ratr0SpriteSheet centi_head_lr_sheet;
UINT8 centi_head_lr_frames[] = {0, 1, 2, 3, 4};
struct Ratr0HWSprite *centi;
int centi_dir = 2;

void main_scene_update(struct Ratr0Scene *this_scene,
                       struct Ratr0DisplayBuffer *backbuffer,
                       UINT8 frames_elapsed)
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
    if (centi_dir > 0 && centi->base_obj.bounds.x >= 200) centi_dir = -centi_dir;
    else if (centi_dir < 0 && centi->base_obj.bounds.x <= 0) centi_dir = -centi_dir;
    centi->base_obj.bounds.x += centi_dir;
}

struct Ratr0Scene *setup_main_scene(Ratr0Engine *eng)
{
    engine = eng;
    // Use the scenes module to create a scene and run that
    struct Ratr0NodeFactory *node_factory = engine->scenes_system->get_node_factory();
    struct Ratr0Scene *main_scene = node_factory->create_scene();
    main_scene->update = main_scene_update;
    ratr0_display_init_copper_list(centi_copper, CENTI_COPPER_SIZE_WORDS,
                                   &CENTI_COPPER_INFO);

    // 1. Read animated sprites from sprite sheet
    ratr0_resources_read_spritesheet(CENTI_HEAD_LR_PATH, &centi_head_lr_sheet);
    centi = ratr0_create_sprite_from_sprite_sheet(&centi_head_lr_sheet, (UINT8) 5, RATR0_ANIM_LOOP_TYPE_PINGPONG);
    // add this sprite to the scene
    centi->base_obj.bounds.x = 0;
    centi->base_obj.bounds.y = 40;
    ratr0_display_set_copperlist(centi_copper, CENTI_COPPER_SIZE_WORDS,
                                 &CENTI_COPPER_INFO);

    main_scene->sprites[main_scene->num_sprites++] = centi;

    return main_scene;
}
