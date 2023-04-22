#include <ratr0/debug_utils.h>
#include <ratr0/data_types.h>
#include <ratr0/engine.h>
#include <ratr0/rendering.h>
#include <ratr0/input.h>
#include <ratr0/timers.h>
#include <ratr0/world.h>

// Amiga specific subsystem access
#include <ratr0/amiga/engine.h>
#include <ratr0/amiga/input.h>
#include <ratr0/amiga/blitter.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mENGINE\033[0m", __VA_ARGS__)

#define TASK_PRIORITY (20)

static Ratr0Engine *engine;
static volatile UWORD *custom_color00 = (volatile UWORD *) 0xdff180;

#define GRID_PATH ("test_assets/basegrid_320x256x3.ts")
struct Ratr0Backdrop *backdrop;  // GLOBAL for performance testing
//#define BOBS_PATH ("test_assets/fox_jump_23x21x3_ni.ts")
#define BOBS_PATH_IL ("test_assets/fox_jump_23x21x3_il.ts")
#define GRID_PATH ("test_assets/basegrid_320x256x3.ts")
#define TILES_PATH_IL ("test_assets/tiles_48x48x3.ts")
struct Ratr0TileSheet bobs_il, tiles_il;
struct Ratr0AmigaSurface tiles_surf;

void ratr0_amiga_engine_startup(Ratr0Engine *eng)
{
    engine = eng;
    engine->game_loop = &ratr0_amiga_engine_game_loop;
    engine->exit = &ratr0_amiga_engine_exit;

    // set high task priority so multitasking does not
    // grab too much CPU
    SetTaskPri(FindTask(NULL), TASK_PRIORITY);

}

void ratr0_amiga_engine_post_startup(void)
{
    struct Ratr0TileSheet grid;
    engine->resource_system->read_tilesheet(GRID_PATH, &grid);
    WaitTOF();  // Make sure we don't get a race condition from the interrupt
    struct Ratr0AmigaSurface *back_buffer, *front_buffer;
    front_buffer = ratr0_amiga_get_front_buffer();
    back_buffer = ratr0_amiga_get_back_buffer();

    // Read BOBS
    //engine->resource_system->read_tilesheet(BOBS_PATH, &bobs);
    engine->resource_system->read_tilesheet(BOBS_PATH_IL, &bobs_il);
    engine->resource_system->read_tilesheet(TILES_PATH_IL, &tiles_il);
    tiles_surf.width = tiles_il.header.width;
    tiles_surf.height = tiles_il.header.height;
    tiles_surf.depth = tiles_il.header.bmdepth;
    tiles_surf.is_interleaved = TRUE;
    tiles_surf.buffer = engine->memory_system->block_address(tiles_il.h_imgdata);

    // Use the scene system to setup the objects of the game
    struct Ratr0NodeFactory *factory = engine->world_system->get_node_factory();
    backdrop = factory->create_backdrop(&grid);
    ratr0_amiga_set_palette(grid.palette, 8);

    OwnBlitter();
    ratr0_amiga_blit_rect(front_buffer, &backdrop->surface, 0, 0, 0, 0, 320, 256);
    ratr0_amiga_blit_rect(back_buffer, &backdrop->surface, 0, 0, 0, 0, 320, 256);
    ratr0_amiga_blit_rect(front_buffer, &tiles_surf, 0, 0, 0, 0, 16, 16);
    ratr0_amiga_blit_rect(back_buffer, &tiles_surf, 0, 16, 16, 0, 16, 16);
    DisownBlitter();
}

enum { GAMESTATE_QUIT, GAMESTATE_RUNNING };
static int game_state = GAMESTATE_RUNNING;

void ratr0_amiga_engine_exit(void)
{
    game_state = GAMESTATE_QUIT;
}

void ratr0_amiga_engine_game_loop(void)
{
    int frame = 0, iteration = 0;
    struct Ratr0AmigaSurface *back_buffer;
    while (game_state != GAMESTATE_QUIT) {
        WaitTOF();
        back_buffer = ratr0_amiga_get_back_buffer();
        // comment in for visual timing the loop iteration
        //*custom_color00 = 0x000;

        frame = iteration < 10 ? 1 : 2;
        OwnBlitter();
        UINT16 bltsize = ratr0_amiga_blit_rect(back_buffer, &backdrop->surface, 32, 0, 32, 0, 16, 16);
        ratr0_amiga_blit_rect_fast(back_buffer, &backdrop->surface, 48, 0, 48, 0, bltsize);
        ratr0_amiga_blit_rect_fast(back_buffer, &backdrop->surface, 32, 16, 32, 16, bltsize);
        ratr0_amiga_blit_rect_fast(back_buffer, &backdrop->surface, 48, 16, 48, 16, bltsize);

        ratr0_amiga_blit_object_il(back_buffer, &bobs_il, 0, frame, 32, 0);
        ratr0_amiga_blit_object_il(back_buffer, &bobs_il, 0, frame, 48, 16);

        DisownBlitter();

        // comment in for visual timing the loop iteration
        //*custom_color00 = 0xf00;
        engine->input_system->update();
        // For now, end when the mouse was clicked. This is just for testing
        UINT32 joystate = engine->input_system->get_joystick_state(0);
        if (joystate != 0) {
            if (joystate & JOY_FIRE0 == JOY_FIRE0) ratr0_amiga_engine_exit();
        }
        iteration++;
        if (iteration == 20) iteration = 0;
    }
}
