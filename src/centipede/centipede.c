/** @file centipede.c - a demo centipede  */
#include <ratr0/ratr0.h>
#include "main_stage.h"

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mMAIN\033[0m", __VA_ARGS__)

// DEBUG: Global action ids
RATR0_ACTION_ID action_fire, action_move_left, action_move_right;

int main(int argc, char **argv)
{
    struct Ratr0DisplayInfo display_init = {
        288, 256,  // viewport
        288, 256,  // display buffer
        2, 2,      // 4 colors, double buffer
        1, TRUE    // 1 frame flip, PAL
    };
    struct Ratr0MemoryConfig mem_config = {
        8192, 20,   // 8k general purpose memory with max 20 mem blocks
        65536, 20  // 64k chip memory with max 20 mem blocks
    };

    Ratr0Engine *engine = ratr0_engine_startup(&mem_config, &display_init);
    // Additional game engine setup: input mapping etc.
    action_move_left = engine->input_system->alloc_action();
    action_move_right = engine->input_system->alloc_action();
    action_fire = engine->input_system->alloc_action();
    engine->input_system->map_input_to_action(action_move_left, RATR0_IC_JS1,
                                              RATR0_INPUT_JS_LEFT);
    engine->input_system->map_input_to_action(action_move_right, RATR0_IC_JS1,
                                              RATR0_INPUT_JS_RIGHT);
    engine->input_system->map_input_to_action(action_fire, RATR0_IC_JS1, RATR0_INPUT_JS_BUTTON0);

    struct Ratr0Scene *main_scene = setup_main_scene(engine);
    engine->scenes_system->set_current_scene(main_scene);

    // START THE ENGINE !!!!
    engine->game_loop();
    engine->shutdown();

    return 0;
}
