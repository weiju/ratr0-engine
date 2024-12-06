/** @file dualplayfield.c - a dual playfield demo  */
#include <ratr0/ratr0.h>
#include "dualplayfield_copper.h"

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mMAIN\033[0m", __VA_ARGS__)
#define PLAYFIELD1_PATH ("assets/playfield1.ts")
#define PLAYFIELD2_PATH ("assets/playfield2.ts")

struct Ratr0CopperListInfo DUALPLAYFIELD_COPPER_INFO = {
    3, 5, 7, 9,
    DUALPLAYFIELD_COPPER_BPLCON0_INDEX, DUALPLAYFIELD_COPPER_BPL1MOD_INDEX,
    DUALPLAYFIELD_COPPER_BPL1PTH_INDEX,
    DUALPLAYFIELD_COPPER_SPR0PTH_INDEX, DUALPLAYFIELD_COPPER_COLOR00_INDEX
};

// DEBUG: Global action ids
RATR0_ACTION_ID action_quit;
static Ratr0Engine *engine;


void main_stage_update(struct Ratr0Stage *this_stage,
                       UINT8 frames_elapsed)
{
    if (ratr0_input_was_action_pressed(action_quit)) {
        ratr0_engine_exit();
    }
}

struct Ratr0TileSheet playfield1_ts, playfield2_ts;
struct Ratr0Surface bg1_surf, bg2_surf;
struct Ratr0Stage *setup_main_stage(Ratr0Engine *eng)
{
    engine = eng;
    // Use the stages module to create a stage and run that
    struct Ratr0NodeFactory *node_factory = ratr0_stages_get_node_factory();
    struct Ratr0Stage *main_stage = node_factory->create_stage();
    main_stage->update = main_stage_update;
    ratr0_display_set_copperlist(dualplayfield_copper,
                                 DUALPLAYFIELD_COPPER_SIZE_WORDS,
                                 &DUALPLAYFIELD_COPPER_INFO);

    BOOL ts_read = ratr0_resources_read_tilesheet(PLAYFIELD1_PATH, &playfield1_ts);
    ts_read = ratr0_resources_read_tilesheet(PLAYFIELD2_PATH, &playfield2_ts);

    // Playfield 1
    ratr0_resources_init_surface_from_tilesheet(&bg1_surf, &playfield1_ts);
    ratr0_display_blit_surface_to_buffers(&bg1_surf, 0, 0, 0);
    ratr0_display_set_palette(playfield1_ts.palette, 8, 0);

    // Playfield 2
    ratr0_resources_init_surface_from_tilesheet(&bg2_surf, &playfield2_ts);
    ratr0_display_blit_surface_to_buffers(&bg2_surf, 1, 0, 0);
    ratr0_display_set_palette(playfield2_ts.palette, 8, 8);

    return main_stage;
}

int main(int argc, char **argv)
{
    struct Ratr0PlayfieldInfo pf_infos[] = {
        {
            320, 256,  // display buffer
            3, 2,      // 4 colors, double buffer
            1          // 1 frame flip
        },
        {
            // second buffer just as the first
            320, 256,
            3, 2,
            1
        }
    };

    struct Ratr0MemoryConfig mem_config = {
        8192, 20,   // 8k general purpose memory with max 20 mem blocks
        65536, 20  // 64k chip memory with max 20 mem blocks
    };

    Ratr0Engine *engine = ratr0_engine_startup(&mem_config,
                                               argc, argv);
    ratr0_display_init_buffers(320, 256, 2, pf_infos);

    // Additional game engine setup: input mapping etc.
    action_quit = ratr0_input_alloc_action();
    ratr0_input_map_input_to_action(action_quit, RATR0_IC_KB, RATR0_KEY_ESCAPE);

    struct Ratr0Stage *main_stage = setup_main_stage(engine);
    ratr0_stages_set_current_stage(main_stage);

    // START THE ENGINE !!!!
    engine->game_loop();
    engine->shutdown();

    return 0;
}
