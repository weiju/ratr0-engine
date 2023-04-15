#include <ratr0/engine.h>
#include <ratr0/world.h>
#include <ratr0/resources.h>

// remove me later
#ifdef AMIGA
#include <stdio.h>
#include <clib/graphics_protos.h>
#include <ratr0/amiga/sprites.h>
#include <ratr0/amiga/display.h>
#include <ratr0/amiga/blitter.h>
#include <ratr0/amiga/world.h>
#endif

int main(int argc, char **argv)
{
    Ratr0Engine *engine = ratr0_engine_startup();

    // Use the scene system to setup the objects of the game
    struct Ratr0NodeFactory *factory = engine->world_system->get_node_factory();
    struct Ratr0Node *main_scene = factory->create_node();
    engine->world_system->set_current_scene(main_scene);

    //struct Ratr0TileSheet sprite;
    struct Ratr0TileSheet grid;
    struct Ratr0TileSheet tiles;
    //struct Ratr0TileSheet bobs;

    //engine->resource_system->read_tilesheet("test_assets/simple_sprite-001.ts", &sprite);
    engine->resource_system->read_tilesheet("test_assets/basegrid_320x256x3.ts", &grid);
    engine->resource_system->read_tilesheet("test_assets/tiles_48x48x3.ts", &tiles);
    //engine->resource_system->read_tilesheet("test_assets/bobs_masked.ts", &bobs);

#ifdef AMIGA

    /*
    UINT8 frames[] = {0};
    struct Ratr0AnimatedAmigaSprite *anim_sprite = ratr0_create_amiga_sprite(&sprite, frames, 1);
    ratr0_amiga_display_set_sprite(0, anim_sprite->sprite_data);*/

    // Set grid as background
    struct Ratr0AmigaRenderContext grid_ctx = {
        grid.header.width, grid.header.height, grid.header.bmdepth, TRUE,
        engine->memory_system->block_address(grid.h_imgdata)
    };

    // Blitter test
    // Blit tile
    struct Ratr0AmigaRenderContext tiles_ctx = {
        tiles.header.width, tiles.header.height, tiles.header.bmdepth, TRUE,
        engine->memory_system->block_address(tiles.h_imgdata)
    };
    ratr0_amiga_set_palette(tiles.palette, 8);
    OwnBlitter();
    struct Ratr0AmigaRenderContext *ctx = ratr0_amiga_get_render_context();
    ratr0_amiga_blit_fast(ctx, &grid_ctx, 0, 0, 0, 0, 320, 256);

    /* Blit command */
    struct Ratr0AmigaBlitCommand cmd;
    ratr0_amiga_make_blit_fast(&cmd, ctx, &tiles_ctx, 16, 16, 0, 0, 16, 16);
    ratr0_amiga_do_blit_command(&cmd);

    // Blit BOB
    /*
    for (int i = 0; i < 12; i++) {
        ratr0_amiga_blit_object(ctx, &bobs, 0, 0, 16 + i * 2, 16 * i);
        }*/
    DisownBlitter();
#endif

    // Then run the game loop
    engine->game_loop();

    // Game cleanup optional
    //engine->resource_system->free_tilesheet_data(&bobs);
    engine->resource_system->free_tilesheet_data(&tiles);
    engine->resource_system->free_tilesheet_data(&grid);
    //engine->resource_system->free_tilesheet_data(&sprite);

    engine->shutdown();

    return 0;
}
