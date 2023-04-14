#include <ratr0/amiga/world.h>
#include <ratr0/amiga/display.h>
#include <ratr0/amiga/sprites.h>

static Ratr0Engine *engine;

// Sprite and bob tables can be in Fastmem
struct Ratr0AnimatedAmigaSprite hw_sprite_table[20];
UINT16 next_hw_sprite = 0;
struct Ratr0AnimatedAmigaBob bob_table[20];
UINT16 next_bob = 0;

void ratr0_amiga_world_startup(Ratr0Engine *eng)
{
    engine = eng;
    next_hw_sprite = next_bob = 0;
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
    result->base_obj.node.class_id = AMIGA_SPRITE;
    result->sprite_data = sprite_data;
    // store sprite information
    return result;
}

struct Ratr0AnimatedAmigaBob *ratr0_create_amiga_bob(struct Ratr0TileSheet *tilesheet,
                                                     UINT8 *frame_indexes, UINT8 num_frames)
{
    struct Ratr0AnimatedAmigaBob *result = &bob_table[next_bob++];
    result->base_obj.node.class_id = AMIGA_BOB;
    result->tilesheet = tilesheet;
    return result;
}
