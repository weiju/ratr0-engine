/** @file main.c */
#include <ratr0/debug_utils.h>
#include <ratr0/engine.h>
#include <ratr0/resources.h>
#include <ratr0/scenes.h>
#include <ratr0/input.h>
#include <ratr0/main_scene.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mMAIN\033[0m", __VA_ARGS__)

// DEBUG: Global action ids
RATR0_ACTION_ID action_fire, action_move_left, action_move_right;

int main(int argc, char **argv)
{
    Ratr0Engine *engine = ratr0_engine_startup();
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
