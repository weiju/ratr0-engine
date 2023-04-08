#include <ratr0/data_types.h>

enum { GAMESTATE_QUIT, GAMESTATE_PLAYING };
static int game_state = GAMESTATE_PLAYING;

volatile UINT8 *ciaa_pra = (volatile UINT8 *) 0xbfe001;

#define  PRA_FIR0_BIT (1 << 6)

void waitmouse(void)
{
  while ((*ciaa_pra & PRA_FIR0_BIT) != 0) ;
}

void ratr0_amiga_engine_game_loop(void)
{
    waitmouse();
    //while (game_state != GAMESTATE_QUIT) {
    //}
}
