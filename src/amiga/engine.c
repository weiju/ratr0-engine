#include <ratr0/debug_utils.h>
#include <ratr0/data_types.h>
#include <ratr0/engine.h>
#include <ratr0/rendering.h>
#include <ratr0/input.h>
#include <ratr0/timers.h>
#include <ratr0/world.h>
#include <ratr0/bitset.h>
#include <ratr0/treeset.h>

// Amiga specific subsystem access
#include <ratr0/amiga/engine.h>
#include <ratr0/amiga/input.h>
#include <ratr0/amiga/blitter.h>
#include <ratr0/amiga/world.h>

#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mENGINE\033[0m", __VA_ARGS__)

#define TASK_PRIORITY (20)

static Ratr0Engine *engine;
static volatile UWORD *custom_color00 = (volatile UWORD *) 0xdff180;

struct Ratr0Backdrop *backdrop;  // GLOBAL for performance testing
//#define BOBS_PATH ("test_assets/fox_jump_23x21x3_ni.ts")
//#define BOBS_PATH_IL ("test_assets/fox_jump_23x21x3.ts")
#define BOBS_PATH_IL ("test_assets/fox_run_20x23x3.ts")
#define GRID_PATH ("test_assets/basegrid_320x256x3.ts")
#define TILES_PATH_IL ("test_assets/tiles_48x48x3.ts")
#define FONT_PATH ("test_assets/arcade_font_1bit.ts")
struct Ratr0TileSheet bobs_il, tiles_il, font;
struct Ratr0AmigaSurface tiles_surf, font_surf;
/**/
struct Ratr0AnimatedAmigaBob *bobs[2];
UINT8 bob_frames[6] = {0, 1, 2, 3, 4, 5};
struct TreeSets *tree_sets;

/**
 * Queue of changed BOBs
 */
static struct TreeSet *bob_queue[2];

// A bitset for each buffer, 10x32 = 320 rectangles each
// representing 20x16 rectangles of 16x16 pixels on a 320x256 frame
// We can map a rectangle (x,y) to a bit index by using
// p_i = y * 20 + x and y = p_i / 20, x = p_i % 20 in row major order

// If we can for example use a base 2 dimension, like the height in this case
// we can use column major order and compute
// p_i = x * 20 + y and x = p_i % 16, x = p_i % 16 in column major order
// which can be faster since we can use bitwise operations instead of
// divisions
#define BITSET_SIZE (10)
UINT32 bitset_arr[2][BITSET_SIZE];
int back, front;

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

    tree_sets = ratr0_startup_tree_sets(eng);

    // data structures for efficient rendering
    front = 0;
    back = 1;
    ratr0_bitset_clear(bitset_arr[back], 10);
    ratr0_bitset_clear(bitset_arr[front], 10);
    bob_queue[front] = tree_sets->get_tree_set();
    bob_queue[back] = tree_sets->get_tree_set();
}

void ratr0_amiga_engine_post_startup(void)
{
    struct Ratr0TileSheet grid;
    engine->resource_system->read_tilesheet(GRID_PATH, &grid);
    WaitTOF();  // synchronize to VB
    struct Ratr0AmigaSurface *back_buffer, *front_buffer;
    front_buffer = ratr0_amiga_get_front_buffer();
    back_buffer = ratr0_amiga_get_back_buffer();

    // Read BOBS
    //engine->resource_system->read_tilesheet(BOBS_PATH, &bobs);
    engine->resource_system->read_tilesheet(BOBS_PATH_IL, &bobs_il);
    engine->resource_system->read_tilesheet(TILES_PATH_IL, &tiles_il);
    engine->resource_system->read_tilesheet(FONT_PATH, &font);

    bobs[0] = ratr0_create_amiga_bob(&bobs_il, bob_frames, 2);
    bobs[0]->base_obj.x = 50; // (48 is cut off !!!) TODO
    bobs[0]->base_obj.y = 16;
    // init animation
    bobs[0]->base_obj.num_frames = 6;
    bobs[0]->base_obj.current_frame = 0;
    bobs[0]->base_obj.current_tick = 0;
    bobs[0]->base_obj.speed = 5;

    bobs[1] = ratr0_create_amiga_bob(&bobs_il, bob_frames, 2);
    bobs[1]->base_obj.x = 83; // (80 is cut off !!!!) TODO
    bobs[1]->base_obj.y = 32;

    bobs[1]->base_obj.num_frames = 6;
    bobs[1]->base_obj.current_frame = 0;
    bobs[1]->base_obj.current_tick = 0;
    bobs[1]->base_obj.speed = 10;

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
}

enum { GAMESTATE_QUIT, GAMESTATE_RUNNING };
static int game_state = GAMESTATE_RUNNING;

void ratr0_amiga_engine_exit(void)
{
    game_state = GAMESTATE_QUIT;
}

static UINT16 dirty_bltsize = 0;
struct Ratr0AmigaSurface *back_buffer;
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


void add_restore_tiles_for_bob(struct Ratr0AnimatedAmigaBob *bob)
{
    // Compute dirty rectangles for the BOB
    // determine first and last horizontal tile positions horizontal and vertical
    int tx0 = bob->base_obj.x >> 4;
    int ty0 = bob->base_obj.y >> 4;
    int txn = (bob->base_obj.x + bob->base_obj.bounding_box.width) >> 4;
    int tyn = (bob->base_obj.y + bob->base_obj.bounding_box.height) >> 4;
    int x, y;
    for (y = ty0; y <= tyn; y++) {
        for (x = tx0; x <= txn; x++) {
            // Add the rectangles to both buffers
            ratr0_bitset_insert(bitset_arr[front], BITSET_SIZE, BITSET_INDEX(x, y));
            ratr0_bitset_insert(bitset_arr[back], BITSET_SIZE, BITSET_INDEX(x, y));
        }
    }
}

/**
 * just fake animation for now until we know it works
 */
static int anim_frames = 0;
BOOL update_bob(struct Ratr0AnimatedAmigaBob *bob)
{
    bob->base_obj.current_tick++;
    if (bob->base_obj.current_tick >= bob->base_obj.speed) {
        // Add an actual frame switcher
        bob->base_obj.current_frame = (bob->base_obj.current_frame + 1) % bob->base_obj.num_frames;
        bob->base_obj.current_tick = 0;
        return TRUE;
    }
    return FALSE;
}

BOOL ptr_lt(void *a, void *b) { return a < b; }
BOOL ptr_eq(void *a, void *b) { return a == b; }

void blit_bob(struct TreeSetNode *node, void *data)
{
    struct Ratr0AnimatedAmigaBob *bob = (struct Ratr0AnimatedAmigaBob *) node->value;
    ratr0_amiga_blit_object_il(back_buffer, bob->tilesheet,
                               0, bob->base_obj.current_frame,
                               bob->base_obj.x,
                               bob->base_obj.y);
}

void ratr0_amiga_engine_game_loop(void)
{
    while (game_state != GAMESTATE_QUIT) {
        WaitTOF();
        back_buffer = ratr0_amiga_get_back_buffer();

        // comment in for visual timing the loop iteration
        //*custom_color00 = 0x000;

        // Simulate updating a changed BOB
        // enqueue dirties
        for (int i = 0; i < 2; i++) {
            if (update_bob(bobs[i])) {
                add_restore_tiles_for_bob(bobs[i]);
                // Add the BOB to the blit set for this and the next frame
                tree_set_insert(bob_queue[back], bobs[i], ptr_lt, ptr_eq);
                tree_set_insert(bob_queue[front], bobs[i], ptr_lt, ptr_eq);
            }
        }

        OwnBlitter();

        // 1. Restore background using the dirty tile set
        ratr0_bitset_iterate(bitset_arr[back], BITSET_SIZE, &process_bit);
        ratr0_bitset_clear(bitset_arr[back], 10); // clear to reset
        // reset bltsize since it's used to determine the first blit of the chain
        dirty_bltsize = 0;

        // 2. Blit updated objects
        tree_set_iterate(bob_queue[back], blit_bob, NULL);
        tree_set_clear(bob_queue[back]);
        DisownBlitter();

        // comment in for visual timing the loop iteration
        //*custom_color00 = 0xf00;
        engine->input_system->update();
        // For now, end when the mouse was clicked. This is just for testing
        UINT32 joystate = engine->input_system->get_joystick_state(0);
        if (joystate != 0) {
            if (joystate & JOY_FIRE0 == JOY_FIRE0) ratr0_amiga_engine_exit();
        }

        // we are done with the back buffer. now swap it to the front
        ratr0_amiga_display_swap_buffers();

        // swap the dirty sets. TODO: It would be better to synchronize the set with
        // the display buffers
        int tmp = front;
        front = back;
        back = tmp;
    }
}
