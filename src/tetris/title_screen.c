/** @file main_stage.c */
#include <stdio.h>
#include <string.h>

#include <ratr0/ratr0.h>
#include "title_screen.h"
#include "main_stage.h"
#include "tetris_copper.h"

static Ratr0Engine *engine = NULL;

static struct Ratr0CopperListInfo TETRIS_COPPER_INFO = {
    3, 5, 7, 9,
    TETRIS_COPPER_BPLCON0_INDEX, TETRIS_COPPER_BPL1MOD_INDEX,
    TETRIS_COPPER_BPL1PTH_INDEX,
    TETRIS_COPPER_SPR0PTH_INDEX, TETRIS_COPPER_COLOR00_INDEX
};
extern RATR0_ACTION_ID action_quit, action_drop;

#define TITLE_PATH_PAL ("tetris/assets/title_screen.ts")
struct Ratr0TileSheet titlescreen_ts;

void title_screen_update(struct Ratr0Scene *this_scene,
                         struct Ratr0DisplayBuffer *backbuffer,
                         UINT8 frame_elapsed) {
    if (engine->input_system->was_action_pressed(action_quit)) {
        ratr0_engine_exit();
    } else if (engine->input_system->was_action_pressed(action_drop)) {
        struct Ratr0Scene *main_scene = setup_main_scene(engine);
        engine->scenes_system->set_current_scene(main_scene);
    }
}

static void _load_resources(void)
{
    // Load background
    struct Ratr0Surface bg_surf;
    BOOL ts_read = ratr0_resources_read_tilesheet(TITLE_PATH_PAL, &titlescreen_ts);
    ratr0_resources_init_surface_from_tilesheet(&bg_surf, &titlescreen_ts);
    ratr0_display_blit_surface_to_buffers(&bg_surf);
    ratr0_display_set_palette(titlescreen_ts.palette, 32, 0);

    // from here we don't need to the memory for the background
    // anymore, we can free the surface and tilesheet
    ratr0_resources_free_tilesheet_data(&titlescreen_ts);
}

void title_screen_on_enter(struct Ratr0Scene *this_scene)
{
    // set new copper list
    ratr0_display_init_copper_list(tetris_copper, TETRIS_COPPER_SIZE_WORDS,
                                   &TETRIS_COPPER_INFO);

    ratr0_display_set_copperlist(tetris_copper, TETRIS_COPPER_SIZE_WORDS,
                                 &TETRIS_COPPER_INFO);

    _load_resources();
}

struct Ratr0Scene *setup_titlescreen_scene(Ratr0Engine *eng)
{
    engine = eng;
    // Use the scenes module to create a scene and run that
    struct Ratr0NodeFactory *node_factory = engine->scenes_system->get_node_factory();
    struct Ratr0Scene *this_scene = node_factory->create_scene();
    this_scene->update = title_screen_update;
    this_scene->on_enter = title_screen_on_enter;

    return this_scene;
}
