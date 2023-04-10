#include <ratr0/engine.h>
#include <ratr0/resources.h>

// remove me later
#include <ratr0/amiga/sprites.h>
#include <ratr0/amiga/display.h>


int main(int argc, char **argv)
{
    Ratr0Engine *engine = ratr0_engine_startup();
    // TODO: Use the scene system to setup the objects of the game
    struct Ratr0TileSheet tilesheet;
    engine->resource_system->read_tilesheet("simple_sprite-001.ts", &tilesheet);
    UINT8 frames[] = {0};
    UINT16 *sprdata = ratr0_amiga_make_sprite_data(&tilesheet, frames, 1);
    ratr0_amiga_display_set_sprite(2, sprdata);

    // Then run the game loop
    engine->game_loop();

    // Game cleanup optional
    engine->resource_system->free_tilesheet_data(&tilesheet);

    engine->shutdown();

    return 0;
}
