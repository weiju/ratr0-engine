#include <ratr0/engine.h>
#include <ratr0/resources.h>

// remove me later
#ifdef AMIGA
#include <ratr0/amiga/sprites.h>
#include <ratr0/amiga/display.h>
#endif

int main(int argc, char **argv)
{
    Ratr0Engine *engine = ratr0_engine_startup();

    // TODO: Use the scene system to setup the objects of the game
#ifdef AMIGA
    struct Ratr0TileSheet tilesheet;
    struct Ratr0TileSheet grid;
    engine->resource_system->read_tilesheet("simple_sprite-001.ts", &tilesheet);
    engine->resource_system->read_tilesheet("basegrid_320x256x3.ts", &grid);
    UINT8 frames[] = {0};
    UINT16 *sprdata = ratr0_amiga_make_sprite_data(&tilesheet, frames, 1);
    ratr0_amiga_display_set_sprite(0, sprdata);
    struct Ratr0AmigaRenderContext grid_ctx = {
        grid.header.width, grid.header.height, grid.header.bmdepth, TRUE,
        engine->memory_system->block_address(grid.h_imgdata)
    };
    ratr0_amiga_set_render_context(&grid_ctx);
#endif

    // Then run the game loop
    engine->game_loop();

#ifdef AMIGA
    // Game cleanup optional
    engine->resource_system->free_tilesheet_data(&grid);
    engine->resource_system->free_tilesheet_data(&tilesheet);
#endif

    engine->shutdown();

    return 0;
}
