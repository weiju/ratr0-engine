/** @file invaders.c - a demo space invaders  */
#include <ratr0/ratr0.h>
#include "inv_main_stage.h"
#include "../default_copper.h"

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mMAIN\033[0m", __VA_ARGS__)

// DEBUG: Global action ids
RATR0_ACTION_ID action_fire, action_move_left, action_move_right;

int main(int argc, char **argv)
{
    struct Ratr0DisplayInit display_init = {
        288, 256,  // viewport
        288, 256,  // display buffer
        2, 2,      // 4 colors, double buffer
        1          // 1 frame flip
    };
    struct Ratr0MemoryConfig mem_config = {
        8192, 20,   // 8k general purpose memory with max 20 mem blocks
        65536, 20  // 64k chip memory with max 20 mem blocks
    };

    Ratr0Engine *engine = ratr0_engine_startup(&mem_config,
                                               argc, argv);
    ratr0_init_display(&display_init);
    ratr0_display_init_copper_list(default_copper, DEFAULT_COPPER_SIZE_WORDS,
                                   &DEFAULT_COPPER_INFO);
    ratr0_display_set_copperlist(default_copper, DEFAULT_COPPER_SIZE_WORDS,
                                 &DEFAULT_COPPER_INFO);

    // Additional game engine setup: input mapping etc.
    action_move_left = ratr0_input_alloc_action();
    action_move_right = ratr0_input_alloc_action();
    action_fire = ratr0_input_alloc_action();
    ratr0_input_map_input_to_action(action_move_left, RATR0_IC_JS1,
                                    RATR0_INPUT_JS_LEFT);
    ratr0_input_map_input_to_action(action_move_right, RATR0_IC_JS1,
                                    RATR0_INPUT_JS_RIGHT);
    ratr0_input_map_input_to_action(action_fire, RATR0_IC_JS1, RATR0_INPUT_JS_BUTTON0);

    struct Ratr0Stage *main_stage = setup_main_stage(engine);
    ratr0_stages_set_current_stage(main_stage);

    // START THE ENGINE !!!!
    engine->game_loop();
    engine->shutdown();

    return 0;
}
