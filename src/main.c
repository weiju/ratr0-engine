#include <ratr0/debug_utils.h>
#include <ratr0/engine.h>
#include <ratr0/world.h>
#include <ratr0/resources.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mMAIN\033[0m", __VA_ARGS__)

// Resources
#define BOBS_PATH_IL ("test_assets/fox_run_20x23x3.ts")
#define GRID_PATH ("test_assets/basegrid_320x256x3.ts")
struct Ratr0TileSheet bobs_il, grid;
struct Ratr0AnimatedAmigaBob *bobs[2];
UINT8 bob_frames[6] = {0, 1, 2, 3, 4, 5};
extern struct Ratr0Backdrop *backdrop;  // GLOBAL for performance testing

int main(int argc, char **argv)
{
    Ratr0Engine *engine = ratr0_engine_startup();

    // Use the world module to create a scene and run that
    struct Ratr0NodeFactory *node_factory = engine->world_system->get_node_factory();
    struct Ratr0Scene *main_scene = node_factory->create_scene();

    engine->resource_system->read_tilesheet(GRID_PATH, &grid);
    ratr0_amiga_set_palette(grid.palette, 8);

    engine->resource_system->read_tilesheet(BOBS_PATH_IL, &bobs_il);
    bobs[0] = (struct Ratr0AnimatedAmigaBob *) node_factory->create_sprite(&bobs_il, bob_frames, 6, 5, FALSE);
    bobs[0]->base_obj.bounds.x = 50;
    bobs[0]->base_obj.bounds.y = 16;

    bobs[1] = (struct Ratr0AnimatedAmigaBob *) node_factory->create_sprite(&bobs_il, bob_frames, 6, 10, FALSE);
    bobs[1]->base_obj.bounds.x = 83;
    bobs[1]->base_obj.bounds.y = 32;

    bobs[0]->base_obj.node.next = (struct Ratr0Node *) bobs[1];
    main_scene->bobs = bobs[0];
    main_scene->backdrop = node_factory->create_backdrop(&grid);

    engine->world_system->set_current_scene(main_scene);

    // START THE ENGINE !!!!
    engine->game_loop();
    engine->shutdown();

    return 0;
}
