/** @file inv_main_stage.c */
#include <stdio.h>
#include <string.h>

#include <ratr0/ratr0.h>
#include <clib/graphics_protos.h>
#include "../default_copper.h"
#include "inv_main_stage.h"

static Ratr0Engine *engine;
extern RATR0_ACTION_ID action_fire, action_move_left, action_move_right;

#define NUM_BOBS (10)
struct Ratr0Bob *bobs[NUM_BOBS];
// Resources
#define BOBS_PATH ("sinvaders/assets/alien001_12x8x2.ts")
#define BOBS2_PATH ("sinvaders/assets/alien001_12x8x1.ts")
#define GRID_PATH ("sinvaders/assets/basegrid_288x256x2.ts")
#define SPRITES_PATH ("sinvaders/assets/all-aliens.spr")
struct Ratr0TileSheet bobs_sheet, bobs2_sheet, grid_sheet;
struct Ratr0SpriteSheet sprite_sheet;
UINT8 bob_frames[2] = {0, 1};
extern struct Ratr0Backdrop *backdrop;  // GLOBAL for performance testing

#define NUM_SPRITE_CONTROL_WORDS (2)
#define SPRITE_DATA_WORDS_PER_ROW (2)
#define NUM_SPRITES (3)
#define SPR0_COLOR00_IDX (16)


void main_scene_update(struct Ratr0Scene *this_scene,
                       struct Ratr0DisplayBuffer *backbuffer,
                       UINT8 frames_elapsed)
{
    // For now, end when the mouse was clicked. This is just for testing
    if (engine->input_system->was_action_pressed(action_fire)) {
        ratr0_engine_exit();
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

    // TEST: blit
    OwnBlitter();
    ratr0_blit_rect_1plane(&backbuffer->surface, &bobs2_sheet, 0, 0, 0, 0);
    DisownBlitter();
}

void copy_sprite(UINT16 *dst, UINT16 *src, UINT16 spr_height)
{
    int idx = 0;
    for (int j = 0; j < spr_height; j++) {
        // 2 words per line
        dst[idx] = src[idx];
        dst[idx + 1] = src[idx + 1];
        idx += SPRITE_DATA_WORDS_PER_ROW;
    }
}


/**
 * We need to copy the alien sheet to a long strip of aliens
 */
void copy_spritesheet_to_sprite()
{
    // copy sprite sheet to test sprite
    UINT16 offset = sprite_sheet.sprite_offsets[0];
    UINT16 *sprdata = (UINT16 *) ratr0_memory_block_address(sprite_sheet.h_imgdata);
    sprdata += offset;
    UINT16 spr_height = sprdata[offset];
    UINT16 spr_attached = sprdata[offset + 1];

    // allocate new sprite block: each sprite is (spr_height * 2) + 2 words long
    // The end of the entire block is ended with 2 control words
    // source and target actually have different layouts: in the source every sprite has its own start
    // and end words, while in our destination we combine the start and end blocks for multiplexing
    UINT16 sprite_block_words = spr_height * 2 + 2;
    Ratr0MemHandle h_newsprite = ratr0_memory_allocate_block(RATR0_MEM_CHIP,
                                                             sprite_block_words * sizeof(UINT16) *
                                                             NUM_SPRITES +
                                                             NUM_SPRITE_CONTROL_WORDS * sizeof(UINT16));

    UINT16 *new_sprite[3];
    UINT16 sprite_pos[3][2] = { {160, 100}, {160, 110}, {160, 120} };
    new_sprite[0] = ratr0_memory_block_address(h_newsprite);

    int dst_idx = 2, src_idx = 2;
    copy_sprite(&new_sprite[0][dst_idx], &sprdata[src_idx], spr_height);
    dst_idx += 18;  // 16 words height + 2 words control
    src_idx += 40;  // 16 words height + 4 words control
    copy_sprite(&new_sprite[0][dst_idx], &sprdata[src_idx], spr_height);
    dst_idx += 18;  // 16 words height + 2 words control
    src_idx += 40;  // 16 words height + 4 words control
    copy_sprite(&new_sprite[0][dst_idx], &sprdata[src_idx], spr_height);

    new_sprite[1] = &(new_sprite[0][NUM_SPRITE_CONTROL_WORDS + spr_height * SPRITE_DATA_WORDS_PER_ROW]);
    new_sprite[2] = &(new_sprite[1][NUM_SPRITE_CONTROL_WORDS + spr_height * SPRITE_DATA_WORDS_PER_ROW]);

    ratr0_display_set_palette(sprite_sheet.colors, sprite_sheet.header.num_colors, SPR0_COLOR00_IDX);

    // set position to copper list
    for (int i = 0; i < 3; i++) {
        ratr0_sprites_set_pos(new_sprite[i], sprite_pos[i][0], sprite_pos[i][1], sprite_pos[i][1] + spr_height);
    }
    // TODO: Point to default copper list
    ratr0_display_set_sprite(default_copper, DEFAULT_COPPER_SIZE_WORDS,
                             &DEFAULT_COPPER_INFO,
                             0, new_sprite[0]);
}

struct Ratr0Scene *setup_main_scene(Ratr0Engine *eng)
{
    engine = eng;
    // Use the scenes module to create a scene and run that
    struct Ratr0NodeFactory *node_factory = engine->scenes_system->get_node_factory();
    struct Ratr0Scene *main_scene = node_factory->create_scene();
    main_scene->update = main_scene_update;

    ratr0_resources_read_tilesheet(GRID_PATH, &grid_sheet);
    ratr0_display_set_palette(grid_sheet.palette, 4, 0);

    ratr0_resources_read_tilesheet(BOBS_PATH, &bobs_sheet);
    ratr0_resources_read_tilesheet(BOBS2_PATH, &bobs2_sheet);
    ratr0_resources_read_spritesheet(SPRITES_PATH, &sprite_sheet);

    copy_spritesheet_to_sprite();
    // SETUP BOBS
    int bobx = 50;
    int boby = 16;
    for (int i = 0; i < NUM_BOBS; i++) {
        bobs[i] = (struct Ratr0Bob *) node_factory->create_sprite(&bobs_sheet, bob_frames, 2, 5, FALSE);
        bobs[i]->base_obj.bounds.x = bobx;
        bobs[i]->base_obj.bounds.y = boby;
        bobx += 16;
        main_scene->bobs[i] = bobs[i];
    }
    main_scene->num_bobs = NUM_BOBS;

    struct Ratr0DisplayBuffer *back_buffer, *front_buffer;
    front_buffer = ratr0_display_get_front_buffer();
    back_buffer = ratr0_display_get_back_buffer();
    main_scene->backdrop = node_factory->create_backdrop(&grid_sheet);
    printf("BACKDROP WIDTH: %d fb width: %d bb width: %d\n",\
           (int) main_scene->backdrop->surface.width,
           (int) front_buffer->surface.width,
           (int) back_buffer->surface.width);
    return main_scene;
}
