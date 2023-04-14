#include <ratr0/amiga/scenes.h>
#include <ratr0/amiga/display.h>
#include <ratr0/amiga/sprites.h>

static Ratr0Engine *engine;

// Sprite table can be in Fastmem
struct Ratr0AnimatedAmigaSprite hw_sprite_table[20];
UINT16 next_hw_sprite = 0;

void ratr0_amiga_scenes_startup(Ratr0Engine *eng)
{
    engine = eng;
}
struct Ratr0AnimatedAmigaSprite *ratr0_create_amiga_sprite(struct Ratr0TileSheet *tilesheet,
                                                           UINT8 *frame_indexes, UINT8 num_frames)
{
    // Note we can only work within the Amiga hardware sprite limitations
    // 1. Reserve memory from engine
    // 2. Convert into sprite data structure and store into allocated memory
    // 3. Return the initialized object
    UINT16 *sprite_data = ratr0_amiga_make_sprite_data(tilesheet, frame_indexes, num_frames);
    struct Ratr0AnimatedAmigaSprite *result = &hw_sprite_table[next_hw_sprite++];
    result->sprite_data = sprite_data;
    // store sprite information
    return result;
}
