#include <ratr0/amiga/blitter.h>

static Ratr0Engine *engine;

void ratr0_amiga_blitter_startup(Ratr0Engine *eng)
{
    engine = eng;
}

void ratr0_amiga_blitter_shutdown(void) { }
