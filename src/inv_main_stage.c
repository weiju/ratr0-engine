/** @file inv_main_stage.c */
#include <stdio.h>
#include <string.h>

#include <ratr0/engine.h>
#include <ratr0/input.h>

#include <ratr0/amiga/engine.h>
#include <ratr0/amiga/display.h>
#include <ratr0/amiga/sprites.h>

#include "inv_main_stage.h"

static Ratr0Engine *engine;
extern RATR0_ACTION_ID action_fire, action_move_left, action_move_right;

#define NUM_BOBS (10)
struct Ratr0AnimatedAmigaBob *bobs[NUM_BOBS];


void main_scene_update(struct Ratr0Scene *this_scene, UINT8 frames_elapsed)
{
    // For now, end when the mouse was clicked. This is just for testing
    if (engine->input_system->was_action_pressed(action_fire)) {
        ratr0_amiga_engine_exit();
    }
    if (engine->input_system->was_action_pressed(action_move_left)) {
        for (int i = 0; i < NUM_BOBS; i++) {
            bobs[i]->base_obj.translate.x--;
        }
    } else if (engine->input_system->was_action_pressed(action_move_right)) {
        for (int i = 0; i < NUM_BOBS; i++) {
            bobs[i]->base_obj.translate.x++;
        }
    }
}

// Resources
#define BOBS_PATH ("sinvaders/alien001_12x8x2.ts")
#define GRID_PATH ("sinvaders/basegrid_288x256x2.ts")
#define SPRITES_PATH ("sinvaders/all-aliens.spr")
struct Ratr0TileSheet bobs_sheet, grid_sheet;
struct Ratr0SpriteSheet sprite_sheet;
UINT8 bob_frames[2] = {0, 1};
extern struct Ratr0Backdrop *backdrop;  // GLOBAL for performance testing

#define NUM_SPRITE_CONTROL_WORDS (2)
#define SPRITE_DATA_WORDS_PER_ROW (2)
#define NUM_SPRITES (2)
#define SPR0_COLOR00_IDX (16)

void copy_sprite(UINT16 *dst, UINT16 *src, UINT16 spr_height)
{
    int idx = NUM_SPRITE_CONTROL_WORDS;
    for (int j = 0; j < spr_height; j++) {
        // 2 words per line
        dst[idx] = src[idx];
        dst[idx + 1] = src[idx + 1];
        idx += SPRITE_DATA_WORDS_PER_ROW;
    }
}


void copy_spritesheet_to_sprite()
{
    // copy sprite sheet to test sprite
    UINT16 offset = sprite_sheet.sprite_offsets[0];
    UINT16 *sprdata = (UINT16 *) engine->memory_system->block_address(sprite_sheet.h_imgdata);
    sprdata += offset;
    UINT16 spr_height = sprdata[offset];
    UINT16 spr_attached = sprdata[offset + 1];

    // allocate new sprite block: each sprite is (spr_height * 2) + 2 words long
    // The end of the entire block is ended with 2 control words
    // source and target actually have different layouts: in the source every sprite has its own start
    // and end words, while in our destination we combine the start and end blocks for multiplexing
    UINT16 sprite_block_words = spr_height * 2 + 2;
    Ratr0MemHandle h_newsprite = engine->memory_system->allocate_block(RATR0_MEM_CHIP,
                                                                       sprite_block_words * sizeof(UINT16) *
                                                                       NUM_SPRITES +
                                                                       NUM_SPRITE_CONTROL_WORDS * sizeof(UINT16));
    UINT16 *new_sprite = engine->memory_system->block_address(h_newsprite);

    int dst_idx = 2, src_idx = 2;
    printf("SPRITE HEIGHT: %d\n", spr_height);
    for (int i = 0; i < NUM_SPRITES; i++) {
        for (int j = 0; j < spr_height; j++) {
            // 2 words per line
            new_sprite[dst_idx] = sprdata[src_idx];
            new_sprite[dst_idx + 1] = sprdata[src_idx + 1];
            src_idx += SPRITE_DATA_WORDS_PER_ROW;
            dst_idx += SPRITE_DATA_WORDS_PER_ROW;
        }
        dst_idx += NUM_SPRITE_CONTROL_WORDS;  // skip 2 control 2 words for multiplexed sprite
        src_idx += NUM_SPRITE_CONTROL_WORDS * 2;  // skip 4 control words for individual sprites
    }

    UINT16 *new_sprite2 = &(new_sprite[NUM_SPRITE_CONTROL_WORDS + spr_height * SPRITE_DATA_WORDS_PER_ROW]);

    ratr0_amiga_set_palette(sprite_sheet.colors, sprite_sheet.header.num_colors, SPR0_COLOR00_IDX);
    // set to copper list
    ratr0_amiga_sprites_set_pos(new_sprite, 160,  100, 100 + spr_height);
    // reused sprite is at base + 4 + sprite_height * 4
    ratr0_amiga_sprites_set_pos(new_sprite2, 160,  110, 110 + spr_height);
    ratr0_amiga_display_set_sprite(0, new_sprite);
}

struct Ratr0Scene *setup_main_scene(Ratr0Engine *eng)
{
    engine = eng;
    // Use the scenes module to create a scene and run that
    struct Ratr0NodeFactory *node_factory = engine->scenes_system->get_node_factory();
    struct Ratr0Scene *main_scene = node_factory->create_scene();
    main_scene->update = main_scene_update;

    engine->resource_system->read_tilesheet(GRID_PATH, &grid_sheet);
    ratr0_amiga_set_palette(grid_sheet.palette, 4, 0);

    engine->resource_system->read_tilesheet(BOBS_PATH, &bobs_sheet);
    engine->resource_system->read_spritesheet(SPRITES_PATH, &sprite_sheet);

    copy_spritesheet_to_sprite();
    // SETUP BOBS
    int bobx = 50;
    int boby = 16;
    for (int i = 0; i < NUM_BOBS; i++) {
        bobs[i] = (struct Ratr0AnimatedAmigaBob *) node_factory->create_sprite(&bobs_sheet, bob_frames, 2, 5, FALSE);
        bobs[i]->base_obj.bounds.x = bobx;
        bobs[i]->base_obj.bounds.y = boby;
        bobx += 16;
        main_scene->bobs[i] = bobs[i];
    }
    main_scene->num_bobs = NUM_BOBS;

    struct Ratr0AmigaSurface *back_buffer, *front_buffer;
    front_buffer = ratr0_amiga_get_front_buffer();
    back_buffer = ratr0_amiga_get_back_buffer();
    main_scene->backdrop = node_factory->create_backdrop(&grid_sheet);
    printf("BACKDROP WIDTH: %d fb width: %d bb width: %d\n", main_scene->backdrop->surface.width,
           front_buffer->width, back_buffer->width);
    return main_scene;
}
