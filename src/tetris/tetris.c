/** @file centipede.c - a demo centipede  */
#include <ratr0/ratr0.h>
#include "main_stage.h"

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mTETRIS\033[0m", __VA_ARGS__)

// Global action ids
// We need actions
//   - left, right, down movement, rotation: cursor keys / joystick direction
//   - drop piece: fire button 1 / space
//   - move/swap to hold: fire button 2 / left shift
RATR0_ACTION_ID action_drop, action_move_left, action_move_right,
    action_move_down,
    action_rotate, action_quit;

int main(int argc, char **argv)
{
    struct Ratr0DisplayInfo display_init = {
        320, 256,  // viewport
        320, 256,  // display buffer
        5, 2,      // 32 colors, double buffer
        //4, 2,      // 16 colors, double buffer
        //3, 2,      // 8 colors, double buffer
        //2, 2,      // 4 colors, double buffer
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
    action_move_down = engine->input_system->alloc_action();
    action_rotate = engine->input_system->alloc_action();
    action_drop = engine->input_system->alloc_action();
    action_quit = engine->input_system->alloc_action();

    engine->input_system->map_input_to_action(action_move_left, RATR0_IC_KB,
                                              RATR0_KEY_LEFT);
    engine->input_system->map_input_to_action(action_move_right, RATR0_IC_KB,
                                              RATR0_KEY_RIGHT);
    engine->input_system->map_input_to_action(action_move_down, RATR0_IC_KB,
                                              RATR0_KEY_DOWN);
    engine->input_system->map_input_to_action(action_rotate, RATR0_IC_KB,
                                              RATR0_KEY_UP);
    engine->input_system->map_input_to_action(action_drop, RATR0_IC_KB,
                                              RATR0_KEY_SPACE);
    engine->input_system->map_input_to_action(action_quit, RATR0_IC_KB,
                                              RATR0_KEY_ESCAPE);

    struct Ratr0Scene *main_scene = setup_main_scene(engine);
    engine->scenes_system->set_current_scene(main_scene);

    // START THE ENGINE !!!!
    engine->game_loop();
    engine->shutdown();

    return 0;
}
