#include <ratr0/debug_utils.h>
#include <ratr0/data_types.h>
#include <ratr0/engine.h>
#include <ratr0/rendering.h>
#include <ratr0/input.h>
#include <ratr0/timers.h>
#include <ratr0/world.h>
#include <ratr0/treeset.h>

// Amiga specific subsystem access
#include <ratr0/amiga/engine.h>
#include <ratr0/amiga/input.h>

#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mENGINE\033[0m", __VA_ARGS__)

#define TASK_PRIORITY (20)

static Ratr0Engine *engine;
static volatile UWORD *custom_color00 = (volatile UWORD *) 0xdff180;

//struct Ratr0Backdrop *backdrop;  // GLOBAL for performance testing
//#define TILES_PATH_IL ("test_assets/tiles_48x48x3.ts")
//#define FONT_PATH ("test_assets/arcade_font_1bit.ts")
//struct Ratr0TileSheet tiles_il, font;
//struct Ratr0AmigaSurface tiles_surf, font_surf;
/**/

void ratr0_amiga_engine_startup(Ratr0Engine *eng)
{
    engine = eng;
    engine->game_loop = &ratr0_amiga_engine_game_loop;
    engine->exit = &ratr0_amiga_engine_exit;

    // set high task priority so multitasking does not
    // grab too much CPU
    SetTaskPri(FindTask(NULL), TASK_PRIORITY);
}

enum { GAMESTATE_QUIT, GAMESTATE_RUNNING };
static int game_state = GAMESTATE_RUNNING;

void ratr0_amiga_engine_exit(void)
{
    game_state = GAMESTATE_QUIT;
}


void ratr0_amiga_engine_game_loop(void)
{
    while (game_state != GAMESTATE_QUIT) {
        WaitTOF();
        engine->world_system->update();

        // comment in for visual timing the loop iteration
        //*custom_color00 = 0xf00;
        engine->input_system->update();
        // For now, end when the mouse was clicked. This is just for testing
        UINT32 joystate = engine->input_system->get_joystick_state(0);
        if (joystate != 0) {
            if (joystate & JOY_FIRE0 == JOY_FIRE0) ratr0_amiga_engine_exit();
        }

        // we are done with the back buffer. now swap it to the front
        ratr0_amiga_display_swap_buffers();
    }
}
