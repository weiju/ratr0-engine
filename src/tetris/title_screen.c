/** @file main_stage.c */
#include <stdio.h>
#include <string.h>

#include <ratr0/ratr0.h>
#include "title_screen.h"
#include "main_stage.h"
#include "../default_copper.h"

static Ratr0Engine *engine = NULL;

extern RATR0_ACTION_ID action_quit, action_drop;

#define TITLE_PATH_PAL ("tetris/assets/title_screen.ts")
struct Ratr0TileSheet titlescreen_ts;

void title_screen_update(struct Ratr0Stage *this_stage,
                         struct Ratr0DisplayBuffer *backbuffer,
                         UINT8 frame_elapsed) {
    if (ratr0_input_was_action_pressed(action_quit)) {
        ratr0_engine_exit();
    } else if (ratr0_input_was_action_pressed(action_drop)) {
        struct Ratr0Stage *main_stage = setup_main_stage(engine);
        engine->stages_system->set_current_stage(main_stage);
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

void title_screen_on_enter(struct Ratr0Stage *this_stage)
{
    // set new copper list
    ratr0_display_init_copper_list(default_copper, DEFAULT_COPPER_SIZE_WORDS,
                                   &DEFAULT_COPPER_INFO);
    ratr0_display_set_copperlist(default_copper, DEFAULT_COPPER_SIZE_WORDS,
                                 &DEFAULT_COPPER_INFO);

    _load_resources();
}

struct Ratr0Stage *setup_titlescreen_stage(Ratr0Engine *eng)
{
    engine = eng;
    // Use the stages module to create a stage and run that
    struct Ratr0NodeFactory *node_factory = engine->stages_system->get_node_factory();
    struct Ratr0Stage *this_stage = node_factory->create_stage();
    this_stage->update = title_screen_update;
    this_stage->on_enter = title_screen_on_enter;

    return this_stage;
}
