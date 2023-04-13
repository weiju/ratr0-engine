#include <ratr0/engine.h>
#include <ratr0/resources.h>

// remove me later
#ifdef AMIGA
#include <ratr0/amiga/sprites.h>
#include <ratr0/amiga/display.h>
#include <ratr0/amiga/blitter.h>
#endif

int main(int argc, char **argv)
{
    Ratr0Engine *engine = ratr0_engine_startup();

    // TODO: Use the scene system to setup the objects of the game
#ifdef AMIGA
    struct Ratr0TileSheet sprite;
    struct Ratr0TileSheet grid;
    struct Ratr0TileSheet tiles;
    engine->resource_system->read_tilesheet("simple_sprite-001.ts", &sprite);
    engine->resource_system->read_tilesheet("basegrid_320x256x3.ts", &grid);
    engine->resource_system->read_tilesheet("tiles_48x48x3.ts", &tiles);
    UINT8 frames[] = {0};
    UINT16 *sprdata = ratr0_amiga_make_sprite_data(&sprite, frames, 1);
    //ratr0_amiga_display_set_sprite(0, sprdata);
    struct Ratr0AmigaRenderContext grid_ctx = {
        grid.header.width, grid.header.height, grid.header.bmdepth, TRUE,
        engine->memory_system->block_address(grid.h_imgdata)
    };
    ratr0_amiga_set_render_context(&grid_ctx);

    // Blitter test
    struct Ratr0AmigaRenderContext tiles_ctx = {
        tiles.header.width, tiles.header.height, tiles.header.bmdepth, TRUE,
        engine->memory_system->block_address(tiles.h_imgdata)
    };
    ratr0_amiga_blit_fast(&grid_ctx, &tiles_ctx, 0, 0, 0, 0, 16, 16);
#endif

    // Then run the game loop
    engine->game_loop();

#ifdef AMIGA
    // Game cleanup optional
    engine->resource_system->free_tilesheet_data(&tiles);
    engine->resource_system->free_tilesheet_data(&grid);
    engine->resource_system->free_tilesheet_data(&sprite);
#endif

    engine->shutdown();

    return 0;
}
