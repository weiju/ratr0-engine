#include <ratr0/data_types.h>
#include <ratr0/engine.h>
#include <ratr0/timers.h>

// Amiga specific subsystem access
#include <ratr0/amiga/input.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#define TASK_PRIORITY (20)
enum { GAMESTATE_QUIT, GAMESTATE_RUNNING };
static int game_state = GAMESTATE_RUNNING;

volatile UINT8 *ciaa_pra = (volatile UINT8 *) 0xbfe001;
static volatile UWORD *custom_color00 = (volatile UWORD *) 0xdff180;
#define  PRA_FIR0_BIT (1 << 6)

static Ratr0Engine *engine;

void ratr0_amiga_engine_startup(Ratr0Engine *eng)
{
    engine = eng;
    // set high task priority so multitasking does not
    // grab too much CPU
    SetTaskPri(FindTask(NULL), TASK_PRIORITY);
}

void ratr0_amiga_engine_game_loop(void)
{
    while (game_state != GAMESTATE_QUIT) {
        // comment in for visual timing the loop iteration
        //*custom_color00 = 0x000;
        // update all subsystems where it makes sense. Obviously it doesn't for
        // memory or resources
        engine->timer_system->update();

        // For now, end when the mouse was clicked
        game_state = ((*ciaa_pra & PRA_FIR0_BIT) != 0) ? GAMESTATE_RUNNING : GAMESTATE_QUIT;
        // comment in for visual timing the loop iteration
        //*custom_color00 = 0xf00;
        WaitTOF();
    }
}
