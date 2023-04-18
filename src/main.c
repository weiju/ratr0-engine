#include <ratr0/debug_utils.h>
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

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mMAIN\033[0m", __VA_ARGS__)

int main(int argc, char **argv)
{
    Ratr0Engine *engine = ratr0_engine_startup();

    // Use the scene system to setup the objects of the game
    struct Ratr0NodeFactory *factory = engine->world_system->get_node_factory();
    struct Ratr0Node *main_scene = factory->create_node();
    engine->world_system->set_current_scene(main_scene);
    struct Ratr0TileSheet grid;
    engine->resource_system->read_tilesheet("test_assets/basegrid_320x256x3.ts", &grid);
    /*
    struct Ratr0Backdrop *backdrop = factory->create_backdrop(&grid);
    engine->world_system->add_child(main_scene, (struct Ratr0Node *) backdrop);
    */
#ifdef AMIGA

    /*
    struct Ratr0TileSheet bobs;
    engine->resource_system->read_tilesheet("test_assets/bobs_masked.ts", &bobs);
    UINT8 frames[] = {0};
    struct Ratr0AnimatedSprite *bob1 = factory->create_animated_sprite(&bobs, frames, 1, FALSE);
    engine->world_system->add_child(main_scene, (struct Ratr0Node *) bob1);
    */
    //struct Ratr0TileSheet sprite;
    //struct Ratr0TileSheet tiles;

    //engine->resource_system->read_tilesheet("test_assets/simple_sprite-001.ts", &sprite);
    //engine->resource_system->read_tilesheet("test_assets/tiles_48x48x3.ts", &tiles);

    /*
    UINT8 frames[] = {0};
    struct Ratr0AnimatedAmigaSprite *anim_sprite = ratr0_create_amiga_sprite(&sprite, frames, 1);
    ratr0_amiga_display_set_sprite(0, anim_sprite->sprite_data);*/

    // Blitter test
    // Blit tile
    /*
    struct Ratr0AmigaSurface tiles_surf = {
        tiles.header.width, tiles.header.height, tiles.header.bmdepth, TRUE,
        engine->memory_system->block_address(tiles.h_imgdata)
    };
    ratr0_amiga_set_palette(tiles.palette, 8);
    */

    struct Ratr0AmigaSurface *disp_surf = ratr0_amiga_get_display_surface();
    // Blit the background
    struct Ratr0AmigaSurface grid_surf = {
        grid.header.width, grid.header.height, grid.header.bmdepth, TRUE,
        engine->memory_system->block_address(grid.h_imgdata)
    };
    //ratr0_amiga_blit_fast(disp_surf, &grid_surf, 0, 0, 0, 0, 320, 256);
    ratr0_amiga_enqueue_blit_fast(disp_surf, &grid_surf, 0, 0, 0, 0, 320, 256);

    /* Test for the blit queue */
    /* Blit command */
    /*
    ratr0_amiga_enqueue_blit_fast(disp_surf, &tiles_surf, 16, 16, 0, 0, 16, 16);
    ratr0_amiga_enqueue_blit_fast(disp_surf, &tiles_surf, 32, 32, 16, 0, 16, 16);
    ratr0_amiga_enqueue_blit_fast(disp_surf, &tiles_surf, 48, 48, 32, 0, 16, 16);
    */
    // Blit BOB
    /*
    for (int i = 0; i < 12; i++) {
        ratr0_amiga_enqueue_blit_object(disp_surf, &bobs, 0, 0, 16 + i * 2, 16 * i);
        }*/
#endif

    // Then run the game loop
    engine->game_loop();

#ifdef AMIGA
    // Game cleanup optional
    //engine->resource_system->free_tilesheet_data(&bobs);
    //engine->resource_system->free_tilesheet_data(&tiles);
    engine->resource_system->free_tilesheet_data(&grid);
    //engine->resource_system->free_tilesheet_data(&sprite);
#endif

    engine->shutdown();

    return 0;
}
