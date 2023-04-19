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

#define GRID_PATH ("test_assets/basegrid_320x256x3.ts")
#define BOBS_PATH ("test_assets/fox-idle_16x16x3.ts")
//#define BOBS_PATH ("test_assets/bobs_masked.ts")
#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mMAIN\033[0m", __VA_ARGS__)

int main(int argc, char **argv)
{
    Ratr0Engine *engine = ratr0_engine_startup();

    struct Ratr0TileSheet grid;
    engine->resource_system->read_tilesheet(GRID_PATH, &grid);

    // Use the scene system to setup the objects of the game
    struct Ratr0NodeFactory *factory = engine->world_system->get_node_factory();
    struct Ratr0Node *main_scene = factory->create_node();
    engine->world_system->set_current_scene(main_scene);
    // Add backdrop to scene
    struct Ratr0Backdrop *backdrop = factory->create_backdrop(&grid);
    engine->world_system->add_child(main_scene, (struct Ratr0Node *) backdrop);

#ifdef AMIGA

    struct Ratr0TileSheet bobs;
    engine->resource_system->read_tilesheet(BOBS_PATH, &bobs);
    ratr0_amiga_set_palette(bobs.palette, 8);
    UINT8 frames[] = {0};
    struct Ratr0AnimatedSprite *bob1 = factory->create_animated_sprite(&bobs, frames, 1, FALSE);
    engine->world_system->add_child(main_scene, (struct Ratr0Node *) bob1);

    /*
    // Blit BOB
    struct Ratr0AmigaSurface *disp_surf = ratr0_amiga_get_display_surface();
    for (int i = 0; i < 12; i++) {
        ratr0_amiga_enqueue_blit_object(disp_surf, &bobs, 0, 0, 16 + i * 2, 16 * i);
        }*/
#endif

    // Then run the game loop
    engine->game_loop();

#ifdef AMIGA
    // Game cleanup optional
    //engine->resource_system->free_tilesheet_data(&bobs);
    engine->resource_system->free_tilesheet_data(&grid);
    //engine->resource_system->free_tilesheet_data(&sprite);
#endif

    engine->shutdown();

    return 0;
}
