#include <ratr0/data_types.h>

volatile UINT8 *ciaa_pra = (volatile UINT8 *) 0xbfe001;

#define  PRA_FIR0_BIT (1 << 6)
enum { GAMESTATE_QUIT, GAMESTATE_PLAYING }; 

void waitmouse(void)
{
  while ((*ciaa_pra & PRA_FIR0_BIT) != 0) ;
}

static int game_state = GAMESTATE_PLAYING;

void ratr0_amiga_engine_game_loop(void)
{
    while (game_state != GAMESTATE_QUIT) {
        if ((*ciaa_pra & PRA_FIR0_BIT) == 0) {
            game_state = GAMESTATE_QUIT;
        }
    }
}
