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
    ratr0_display_init_copper_list(centi_copper, CENTI_COPPER_SIZE_WORDS,
                                   &CENTI_COPPER_INFO);

    // 1. Read animated sprites from sprite sheet
    engine->resource_system->read_spritesheet(CENTI_HEAD_LR_PATH, &centi_head_lr_sheet);
    UWORD *sprite_data = (UWORD *) engine->memory_system->block_address(centi_head_lr_sheet.h_imgdata);
    sprite_data += centi_head_lr_sheet.sprite_offsets[1];

    UWORD spr0_x = 150, spr0_y = 0x5c, spr0_height = 8;
    ratr0_sprites_set_pos(sprite_data, spr0_x, spr0_y, spr0_y + spr0_height);
    // All sprites are pointed to null sprite, so now we can set the sprite pointer
    ratr0_display_set_sprite(centi_copper, CENTI_COPPER_SIZE_WORDS,
                             &CENTI_COPPER_INFO,
                             0, sprite_data);

    ratr0_display_set_copperlist(centi_copper, CENTI_COPPER_SIZE_WORDS,
                                 &CENTI_COPPER_INFO);

    // can we do this in one step ?
    /*
    engine->resource_system->read_tilesheet(CENTI_HEAD_LR_PATH, &centi_head_lr_sheet);
    struct Ratr0HWSprite *anim_sprite = (struct Ratr0HWSprite *)
        node_factory->create_sprite(&centi_head_lr_sheet,
                                    centi_head_lr_frames, 5, 1, TRUE);
    main_scene->sprites[0] = anim_sprite;
    main_scene->num_sprites = 1;

    anim_sprite->base_obj.bounds.x = 0;
    anim_sprite->base_obj.bounds.y = 10;
    */

    //_copperlist_set_sprite(1, sprdata0);

    return main_scene;
}
