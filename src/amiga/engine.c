#include <ratr0/data_types.h>
#include <ratr0/engine.h>
#include <ratr0/timers.h>
#include <clib/graphics_protos.h>

enum { GAMESTATE_QUIT, GAMESTATE_PLAYING };
static int game_state = GAMESTATE_PLAYING;

volatile UINT8 *ciaa_pra = (volatile UINT8 *) 0xbfe001;

#define  PRA_FIR0_BIT (1 << 6)

void waitmouse(void)
{
  while ((*ciaa_pra & PRA_FIR0_BIT) != 0) ;
}

static Ratr0Engine *engine;

void ratr0_amiga_engine_startup(Ratr0Engine *eng)
{
    engine = eng;
}
void ratr0_amiga_engine_game_loop(void)
{
    waitmouse();
    while (game_state != GAMESTATE_QUIT) {
        WaitTOF();

        // update all subsystems where it makes sense. Obviously it doesn't for
        // memory or resources
        engine->timer_system->update();
    }
}
