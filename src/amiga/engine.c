#include <ratr0/debug_utils.h>
#include <ratr0/data_types.h>
#include <ratr0/engine.h>
#include <ratr0/rendering.h>
#include <ratr0/input.h>
#include <ratr0/timers.h>
#include <ratr0/world.h>
#include <ratr0/bitset.h>

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

struct Ratr0Backdrop *backdrop;  // GLOBAL for performance testing
//#define BOBS_PATH ("test_assets/fox_jump_23x21x3_ni.ts")
#define BOBS_PATH_IL ("test_assets/fox_jump_23x21x3_il.ts")
#define GRID_PATH ("test_assets/basegrid_320x256x3.ts")
#define TILES_PATH_IL ("test_assets/tiles_48x48x3.ts")
#define FONT_PATH ("test_assets/arcade_font_1bit.ts")
struct Ratr0TileSheet bobs_il, tiles_il, font;
struct Ratr0AmigaSurface tiles_surf, font_surf;

// A bitset for each buffer, 10x32 = 320 rectangles each
// representing 20x16 rectangles of 16x16 pixels on a 320x256 frame
// We can map a rectangle (x,y) to a bit index by using
// p_i = y * 20 + x and y = p_i / 20, x = p_i % 20 in row major order

// If we can for example use a base 2 dimension, like the height in this case
// we can use column major order and compute
// p_i = x * 20 + y and x = p_i % 16, x = p_i % 16 in column major order
// which can be faster since we can use bitwise operations instead of
// divisions
#define DIRTY_QUEUE
#define BITSET_SIZE (10)
UINT32 bitset_arr_01[BITSET_SIZE];
UINT32 bitset_arr_02[BITSET_SIZE];

#define BITSET_INDEX(x, y) (y * 20 + x)
#define BITSET_X(idx) (idx % 20)
#define BITSET_Y(idx) (idx / 20)

void ratr0_amiga_engine_startup(Ratr0Engine *eng)
{
    engine = eng;
    engine->game_loop = &ratr0_amiga_engine_game_loop;
    engine->exit = &ratr0_amiga_engine_exit;

    // set high task priority so multitasking does not
    // grab too much CPU
    SetTaskPri(FindTask(NULL), TASK_PRIORITY);

    ratr0_bitset_clear(bitset_arr_01, 10);
    ratr0_bitset_clear(bitset_arr_02, 10);
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
    engine->resource_system->read_tilesheet(FONT_PATH, &font);

    // Tile surface
    tiles_surf.width = tiles_il.header.width;
    tiles_surf.height = tiles_il.header.height;
    tiles_surf.depth = tiles_il.header.bmdepth;
    tiles_surf.is_interleaved = TRUE;
    tiles_surf.buffer = engine->memory_system->block_address(tiles_il.h_imgdata);

    // Font surface
    font_surf.width = font.header.width;
    font_surf.height = font.header.height;
    font_surf.depth = font.header.bmdepth;
    font_surf.is_interleaved = TRUE;
    font_surf.buffer = engine->memory_system->block_address(font.h_imgdata);

    // Use the scene system to setup the objects of the game
    struct Ratr0NodeFactory *factory = engine->world_system->get_node_factory();
    backdrop = factory->create_backdrop(&grid);
    ratr0_amiga_set_palette(grid.palette, 8);

    OwnBlitter();
    ratr0_amiga_blit_rect(front_buffer, &backdrop->surface, 0, 0, 0, 0, 320, 256);
    ratr0_amiga_blit_rect(back_buffer, &backdrop->surface, 0, 0, 0, 0, 320, 256);
    ratr0_amiga_blit_rect(front_buffer, &tiles_surf, 0, 0, 0, 0, 16, 16);
    ratr0_amiga_blit_rect(back_buffer, &tiles_surf, 0, 16, 16, 0, 16, 16);

    // Both buffers, Write CAT
    ratr0_amiga_blit_8x8(front_buffer, &font_surf, 16, 48, 'C', 2);
    ratr0_amiga_blit_8x8(back_buffer, &font_surf, 16, 48, 'C', 2);
    ratr0_amiga_blit_8x8(front_buffer, &font_surf, 24, 48, 'A', 2);
    ratr0_amiga_blit_8x8(back_buffer, &font_surf, 24, 48, 'A', 2);
    ratr0_amiga_blit_8x8(front_buffer, &font_surf, 32, 48, 'T', 2);
    ratr0_amiga_blit_8x8(back_buffer, &font_surf, 32, 48, 'T', 2);


    DisownBlitter();

    // Insert the dirty rectangles
    ratr0_bitset_insert(bitset_arr_01, BITSET_SIZE, BITSET_INDEX(32 / 16, 0));
    ratr0_bitset_insert(bitset_arr_01, BITSET_SIZE, BITSET_INDEX(48 / 16, 0));
    ratr0_bitset_insert(bitset_arr_01, BITSET_SIZE, BITSET_INDEX(32 / 16, 16 / 16));
    ratr0_bitset_insert(bitset_arr_01, BITSET_SIZE, BITSET_INDEX(48 / 16, 16 / 16));
    ratr0_bitset_insert(bitset_arr_01, BITSET_SIZE, BITSET_INDEX(64 / 16, 16 / 16));
    ratr0_bitset_insert(bitset_arr_01, BITSET_SIZE, BITSET_INDEX(48 / 16, 32 / 16));
    ratr0_bitset_insert(bitset_arr_01, BITSET_SIZE, BITSET_INDEX(64 / 16, 32 / 16));
}

enum { GAMESTATE_QUIT, GAMESTATE_RUNNING };
static int game_state = GAMESTATE_RUNNING;

void ratr0_amiga_engine_exit(void)
{
    game_state = GAMESTATE_QUIT;
}

static UINT16 dirty_bltsize = 0;
struct Ratr0AmigaSurface *back_buffer, *front_buffer;
void process_bit(UINT16 index)
{
    UINT16 x = BITSET_X(index) << 4;  // * 16
    UINT16 y = BITSET_Y(index) << 4;
    if (!dirty_bltsize) {
        dirty_bltsize = ratr0_amiga_blit_rect(back_buffer, &backdrop->surface,
                                              x, y, x, y,
                                              16, 16);
    } else {
        ratr0_amiga_blit_rect_fast(back_buffer, &backdrop->surface, x, y, x, y, dirty_bltsize);
    }
}

void ratr0_amiga_engine_game_loop(void)
{
    int frame = 0, iteration = 0;
    while (game_state != GAMESTATE_QUIT) {
        WaitTOF();
        back_buffer = ratr0_amiga_get_back_buffer();
        front_buffer = ratr0_amiga_get_front_buffer();
        // comment in for visual timing the loop iteration
        *custom_color00 = 0x000;

        frame = iteration < 5 ? 1 : 2;
        OwnBlitter();
#ifdef DIRTY_QUEUE
        ratr0_bitset_iterate(bitset_arr_01, BITSET_SIZE, &process_bit);
#else
        UINT16 bltsize = ratr0_amiga_blit_rect(back_buffer, &backdrop->surface, 32, 0, 32, 0, 16, 16);
        ratr0_amiga_blit_rect_fast(back_buffer, &backdrop->surface, 48, 0, 48, 0, bltsize);
        ratr0_amiga_blit_rect_fast(back_buffer, &backdrop->surface, 32, 16, 32, 16, bltsize);
        ratr0_amiga_blit_rect_fast(back_buffer, &backdrop->surface, 48, 16, 48, 16, bltsize);
        ratr0_amiga_blit_rect_fast(back_buffer, &backdrop->surface, 64, 16, 64, 16, bltsize);
        ratr0_amiga_blit_rect_fast(back_buffer, &backdrop->surface, 48, 32, 48, 32, bltsize);
        ratr0_amiga_blit_rect_fast(back_buffer, &backdrop->surface, 64, 32, 64, 32, bltsize);
#endif
        ratr0_amiga_blit_object_il(back_buffer, &bobs_il, 0, frame, 32, 0);
        ratr0_amiga_blit_object_il(back_buffer, &bobs_il, 0, frame, 48, 16);
        DisownBlitter();
        dirty_bltsize = 0;

        // comment in for visual timing the loop iteration
        *custom_color00 = 0xf00;
        engine->input_system->update();
        // For now, end when the mouse was clicked. This is just for testing
        UINT32 joystate = engine->input_system->get_joystick_state(0);
        if (joystate != 0) {
            if (joystate & JOY_FIRE0 == JOY_FIRE0) ratr0_amiga_engine_exit();
        }
        iteration++;
        if (iteration == 10) iteration = 0;

        // we are done with the back buffer. now swap it to the front
        ratr0_amiga_display_swap_buffers();
    }
}
