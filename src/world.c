#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/engine.h>
#include <ratr0/bitset.h>
#include <ratr0/world.h>

#ifdef AMIGA
#include <ratr0/amiga/world.h>
#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[33;1mWORLD\033[0m", __VA_ARGS__)
#else
#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[34mWORLD\033[0m", __VA_ARGS__)
#endif

/*
 * The world system is where games are implemented on a high level and tied together.
 * It builds on all the subsystems that deal with low-level aspects and integrates
 * with the scripting system.
 */
static struct Ratr0WorldSystem world_system;
static struct Ratr0NodeFactory node_factory;
static Ratr0Engine *engine;
static struct Ratr0Node *current_scene;

#define BITSET_SIZE (10)
#define TILES_PER_ROW (16)
UINT32 bitset_arr[BITSET_SIZE];  // 320 elements

static void ratr0_world_shutdown(void);
static void ratr0_world_update(void);
static void ratr0_world_set_current_scene(struct Ratr0Node *);


/*
void _bitset_enqueue_tile(UINT16 index)
{
    // TODO: enqueue tile
}

static void ratr0_world_update_node(struct Ratr0Node *cur)
{
    // 1. do an update action on the current node
    switch (cur->class_id) {
    case BACKDROP:
        {
            // Render backdrop
            // We actually need a system to manage the integrity of the
            // backdrop, e.g. restoring dirty rectangles etc.
            struct Ratr0Backdrop *backdrop = (struct Ratr0Backdrop *) cur;
#ifdef AMIGA
            if (!backdrop->was_drawn) {
                // Put it in the queue
                ratr0_amiga_enqueue_blit_rect(ratr0_amiga_get_back_buffer(),
                                              &backdrop->surface,
                                              0, 0, 0, 0,
                                              backdrop->surface.width, backdrop->surface.height);
                backdrop->was_drawn = TRUE;
            } else {
                // Submit dirty rectangles to blitter queue
                ratr0_bitset_iterate(bitset_arr, BITSET_SIZE, _bitset_enqueue_tile);
                // Clear dirty rect set at the end
                ratr0_bitset_clear(bitset_arr, BITSET_SIZE);
            }
#endif // AMIGA
        }
        break;
    case AMIGA_BOB:
#ifdef AMIGA
        {
            // TODO: we only need to do stuff if anything changed
            //   1. animation frame changed
            //   2. position changed
            //   3. overlap with another BOB object

            struct Ratr0AnimatedAmigaBob *bob = (struct Ratr0AnimatedAmigaBob *) cur;
            // TODO: map the current animation frame to a tile position
            UINT16 tilex = 0, tiley = 0;
            UINT16 dstx = bob->base_obj.x, dsty = bob->base_obj.y;
            ratr0_amiga_enqueue_blit_object(ratr0_amiga_get_back_buffer(),
                                            bob->tilesheet, tilex, tiley,
                                            dstx, dsty);

            // TODO: dirty rectangle algorithm to restore destroyed parts
            // for this, we need to know the tiles, we are obscuring
            // Start with the top left corner: That rectangle needs to go in first
            int rx_0 = dstx / 16;
            int ry_0 = dsty / 16;
            // The tilesheet is declared as 16 pixel wider because of the
            // shift padding. So this would add another column of dirty rectangles
            int rx_m = (dstx + bob->tilesheet->header.tile_width - 16) / 16;
            int ry_n = (dsty + bob->tilesheet->header.tile_height) / 16;

            // now we have the bounding box of the bob and now we can add all
            // obscured rectangles to the dirty set [rx_0..rx_n]x[ry_0..ry_n]
            //PRINT_DEBUG("ADD DIRTY FOR: (%d,%d) rx_m: %d, ry_n: %d",
            //            rx_0, ry_0, rx_m, ry_n);
            for (int ry_i = ry_0; ry_i <= ry_n; ry_i++) {
                for (int rx_j = rx_0; rx_j <= rx_m; rx_j++) {
                    // add dirty rectangle
                    ratr0_bitset_insert(bitset_arr, BITSET_SIZE, ry_i * TILES_PER_ROW + rx_j);
                }
            }
        }
#endif
        break;
    default:
        //PRINT_DEBUG("UNKNOWN CLASS ID: %d", (int) cur->class_id);
        break;
    }

    // 2. call update on all children
    struct Ratr0Node *cur_child = cur->children;
    while (cur_child) {
        ratr0_world_update_node(cur_child);
        cur_child = cur_child->next;
    }
}
*/

void ratr0_world_add_child(struct Ratr0Node *parent, struct Ratr0Node *child)
{
    if (!parent->children) parent->children = child;
   else {
        struct Ratr0Node *cur = parent->children;
        while (cur->next) cur = cur->next;
        cur->next = child;
    }
}

/**
 * Node factory
 */
// *do not* make the arrays static !!!! Otherwise bad things happen when you
// try accessing thim outside of the module
struct Ratr0Node _nodes[10];
static UINT16 next_node = 0;

struct Ratr0Backdrop _backdrops[2]; // we don't have many of those
static UINT16 next_backdrop = 0;

static struct Ratr0NodeFactory *ratr0_world_get_node_factory(void) { return &node_factory; }

/**
 * Base node initialization
 */
void ratr0_world_init_base_node(struct Ratr0Node *node, UINT16 clsid)
{
    node->class_id = clsid;
    node->children = node->next = NULL;
}

static struct Ratr0Node *ratr0_world_create_node(void)
{
    struct Ratr0Node *result = &_nodes[next_node++];
    ratr0_world_init_base_node(result, RATR0_NODE);
    return result;
}

static struct Ratr0AnimatedSprite *ratr0_nf_create_animated_sprite(struct Ratr0TileSheet *,
                                                                   UINT8 *, UINT8, BOOL);

struct Ratr0Backdrop *ratr0_nf_create_backdrop(struct Ratr0TileSheet *tilesheet);

struct Ratr0WorldSystem *ratr0_world_startup(Ratr0Engine *eng)
{
    engine = eng;
    current_scene = NULL;

#ifdef AMIGA
    ratr0_amiga_world_startup(eng, &world_system);
#else
    world_system.update = &ratr0_world_update;
#endif
    world_system.shutdown = &ratr0_world_shutdown;
    world_system.set_current_scene = &ratr0_world_set_current_scene;
    // world_system.update_node = &ratr0_world_update_node;
    world_system.add_child = &ratr0_world_add_child;

    // Node factory
    node_factory.create_node = &ratr0_world_create_node;
    node_factory.create_animated_sprite = &ratr0_nf_create_animated_sprite;
    node_factory.create_backdrop = &ratr0_nf_create_backdrop;
    world_system.get_node_factory = &ratr0_world_get_node_factory;

    PRINT_DEBUG("Startup finished.");
    return &world_system;
}

static void ratr0_world_shutdown(void)
{
    PRINT_DEBUG("Shutdown finished.");
}

static void ratr0_world_set_current_scene(struct Ratr0Node *node)
{
    current_scene = node;
}

static struct Ratr0AnimatedSprite *ratr0_nf_create_animated_sprite(struct Ratr0TileSheet *tilesheet,
                                                                   UINT8 *frame_indexes, UINT8 num_indexes,
                                                                   BOOL is_hw)
{
#ifdef AMIGA
    if (is_hw) {
        return (struct Ratr0AnimatedSprite *) ratr0_create_amiga_sprite(tilesheet, frame_indexes, num_indexes);
    } else {
        return (struct Ratr0AnimatedSprite *) ratr0_create_amiga_bob(tilesheet, frame_indexes, num_indexes);
    }
    return NULL;
#else
    // TODO
    return NULL;
#endif
}

struct Ratr0Backdrop *ratr0_nf_create_backdrop(struct Ratr0TileSheet *tilesheet)
{
    struct Ratr0Backdrop *result = &_backdrops[next_backdrop++];
    ratr0_world_init_base_node((struct Ratr0Node *) result, BACKDROP);
#ifdef AMIGA
    // Initialize the backdrop
    result->surface.width = tilesheet->header.width;
    result->surface.height = tilesheet->header.height;
    result->surface.depth = tilesheet->header.bmdepth;
    result->surface.is_interleaved = tilesheet->header.flags & TSFLAGS_NON_INTERLEAVED == 0;
    result->surface.buffer = engine->memory_system->block_address(tilesheet->h_imgdata);
    result->was_drawn = FALSE;
#endif
    return result;
}

static void ratr0_world_update(void) { }
