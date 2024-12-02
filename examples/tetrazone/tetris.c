/** @file centipede.c - a demo centipede  */
#include <ratr0/ratr0.h>
#include "main_stage.h"
#include "title_screen.h"
#include "hiscore_screen.h"
#include "default_copper.h"

/** \brief default copper list indexes */
struct Ratr0CopperListInfo DEFAULT_COPPER_INFO = {
    3, 5, 7, 9,
    DEFAULT_COPPER_BPLCON0_INDEX, DEFAULT_COPPER_BPL1MOD_INDEX,
    DEFAULT_COPPER_BPL1PTH_INDEX,
    DEFAULT_COPPER_SPR0PTH_INDEX, DEFAULT_COPPER_COLOR00_INDEX
};

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mTETRIS\033[0m", __VA_ARGS__)

// Global action ids
// We need actions
//   - left, right, down movement, rotation: cursor keys / joystick direction
//   - drop piece: fire button 1 / space
//   - move/swap to hold: fire button 2 / left shift
RATR0_ACTION_ID action_drop, action_move_left, action_move_right,
    action_move_down, action_rotate_right, action_rotate_left,
    action_hold, action_music,
    action_quit;

struct Ratr0Stage *main_stage, *title_screen, *hiscore_screen;

int main(int argc, char **argv)
{
    struct Ratr0MemoryConfig mem_config = {
        8192, 40,   // 8k general purpose memory with max 20 mem blocks
        //131072, 40  // 128k chip memory with max 20 mem blocks
        262144, 40  // 256k chip memory with max 40 mem blocks
    };

    struct Ratr0DisplayInit display_init = {
        320, 256,  // viewport
        320, 256,  // display buffer
        5, 2,      // 32 colors, double buffer
        1    // 1 frame flip
    };
    Ratr0Engine *engine = ratr0_engine_startup(&mem_config,
                                               argc, argv);
    ratr0_init_display(&display_init);

    // Additional game engine setup: input mapping etc.
    action_move_left = ratr0_input_alloc_action();
    action_move_right = ratr0_input_alloc_action();
    action_move_down = ratr0_input_alloc_action();
    action_rotate_right = ratr0_input_alloc_action();
    action_rotate_left = ratr0_input_alloc_action();
    action_hold = ratr0_input_alloc_action();
    action_drop = ratr0_input_alloc_action();
    action_quit = ratr0_input_alloc_action();
    action_music = ratr0_input_alloc_action();

    ratr0_input_map_input_to_action(action_move_left, RATR0_IC_KB,
                                    RATR0_KEY_LEFT);
    ratr0_input_map_input_to_action(action_move_left, RATR0_IC_JS1,
                                    RATR0_INPUT_JS_LEFT);

    ratr0_input_map_input_to_action(action_move_right, RATR0_IC_KB,
                                    RATR0_KEY_RIGHT);
    ratr0_input_map_input_to_action(action_move_right, RATR0_IC_JS1,
                                    RATR0_INPUT_JS_RIGHT);

    ratr0_input_map_input_to_action(action_move_down, RATR0_IC_KB,
                                    RATR0_KEY_DOWN);
    ratr0_input_map_input_to_action(action_move_down, RATR0_IC_JS1,
                                    RATR0_INPUT_JS_DOWN);

    ratr0_input_map_input_to_action(action_rotate_right, RATR0_IC_KB,
                                    RATR0_KEY_UP);
    ratr0_input_map_input_to_action(action_rotate_right, RATR0_IC_JS1, RATR0_INPUT_JS_BUTTON0);

    // TODO: consider something like Z or Q to rotate left
    ratr0_input_map_input_to_action(action_drop, RATR0_IC_KB,
                                    RATR0_KEY_SPACE);
    ratr0_input_map_input_to_action(action_drop, RATR0_IC_JS1,
                                    RATR0_INPUT_JS_UP);

    ratr0_input_map_input_to_action(action_hold, RATR0_IC_KB,
                                    RATR0_KEY_RAW_Z);
    ratr0_input_map_input_to_action(action_music, RATR0_IC_KB,
                                    RATR0_KEY_RAW_M);

    // non-game actions
    ratr0_input_map_input_to_action(action_quit, RATR0_IC_KB,
                                    RATR0_KEY_ESCAPE);

    main_stage = setup_main_stage(engine);
    title_screen = setup_titlescreen_stage(engine);
    hiscore_screen = setup_hiscorescreen_stage(engine);
    ratr0_stages_set_current_stage(title_screen);

    // START THE ENGINE !!!!
    engine->game_loop();
    engine->shutdown();

    return 0;
}
