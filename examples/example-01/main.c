/** @file main.c */
#include <ratr0/ratr0.h>
#include "main_scene.h"
#include "default_copper.h"

struct Ratr0CopperListInfo DEFAULT_COPPER_INFO = {
    3, 5, 7, 9,
    DEFAULT_COPPER_BPLCON0_INDEX, DEFAULT_COPPER_BPL1MOD_INDEX,
    DEFAULT_COPPER_BPL1PTH_INDEX,
    DEFAULT_COPPER_SPR0PTH_INDEX, DEFAULT_COPPER_COLOR00_INDEX
};

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mMAIN\033[0m", __VA_ARGS__)

// DEBUG: Global action ids
RATR0_ACTION_ID action_fire, action_move_left, action_move_right, action_exit;

int main(int argc, char **argv)
{
    struct Ratr0DisplayInit display_init = {
        320, 256,  // viewport
        320, 256,  // display buffer
        3, 2,      // 8 colors, double buffer
        1    // 1 frame flip
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
    action_exit = ratr0_input_alloc_action();

    ratr0_input_map_input_to_action(action_move_left, RATR0_IC_JS1,
                                    RATR0_INPUT_JS_LEFT);
    ratr0_input_map_input_to_action(action_move_right, RATR0_IC_JS1,
                                    RATR0_INPUT_JS_RIGHT);
    ratr0_input_map_input_to_action(action_exit, RATR0_IC_JS1, RATR0_INPUT_JS_BUTTON0);
    ratr0_input_map_input_to_action(action_exit, RATR0_IC_KB, RATR0_KEY_ESCAPE);

    struct Ratr0Stage *main_stage = setup_main_stage(engine);
    ratr0_stages_set_current_stage(main_stage);

    // START THE ENGINE !!!!
    engine->game_loop();
    engine->shutdown();

    return 0;
}
