#include <ratr0/amiga/scenes.h>
#include <ratr0/amiga/display.h>

static Ratr0Engine *engine;

void ratr0_amiga_scenes_startup(Ratr0Engine *eng)
{
    engine = eng;
}
struct Ratr0AnimatedAmigaSprite *ratr0_create_amiga_sprite(struct Ratr0TileSheet *tilesheet,
                                                           UINT8 *frame_indexes)
{
    // Note we can only work within the Amiga hardware sprite limitations
    // 1. Reserve memory from engine
    // 2. Convert into sprite data structure and store into allocated memory
    // 3. Return the initialized object
    return NULL;
}
