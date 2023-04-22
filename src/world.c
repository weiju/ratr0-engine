#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/world.h>
#include <ratr0/amiga/world.h>

#ifdef AMIGA
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

static void ratr0_world_shutdown(void);
static void ratr0_world_update(void);
static void ratr0_world_set_current_scene(struct Ratr0Node *);

/**
 * Adds a dirty rectangle to the set of dirty rectangles.
 * Since we need to translate the rectangles into blitter commands,
 * just make sure we have unique elements, order doesn't matter
 */
void ratr0_world_add_dirty_rect(UINT16 rx, UINT16 ry)
{
    // TODO
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
                ratr0_amiga_enqueue_blit_fast(ratr0_amiga_get_display_surface(),
                                              &backdrop->surface,
                                              0, 0, 0, 0,
                                              backdrop->surface.width, backdrop->surface.height);
                backdrop->was_drawn = TRUE;
            }
#endif
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
            ratr0_amiga_enqueue_blit_object(ratr0_amiga_get_display_surface(),
                                            bob->tilesheet, tilex, tiley,
                                            dstx, dsty);

            // TODO: dirty rectangle algorithm to restore destroyed parts
            // for this, we need to know the tiles, we are obscuring
            // Start with the top left corner: That rectangle needs to go in first
            int rx_0 = dstx / 16;
            int ry_0 = dsty / 16;
            int rx_m = (dstx + bob->tilesheet->header.tile_width) / 16;
            int ry_n = (dstx + bob->tilesheet->header.tile_height) / 16;

            // now we have the bounding box of the bob and now we can add all
            // obscured rectangles to the dirty set [rx_0..rx_n]x[ry_0..ry_n]
            for (int ry_i = ry_0; ry_i <= ry_n; ry_i++) {
                for (int rx_j = rx_0; rx_j <= rx_m; rx_j++) {
                    // add dirty rectangle (rxi, rxg)
                    ratr0_world_add_dirty_rect(rx_j, ry_i);
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
    ratr0_amiga_world_startup(eng);
#endif
    world_system.shutdown = &ratr0_world_shutdown;
    world_system.update = &ratr0_world_update;
    world_system.set_current_scene = &ratr0_world_set_current_scene;
    world_system.update_node = &ratr0_world_update_node;
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

static void ratr0_world_update(void)
{
    if (current_scene) {
        ratr0_world_update_node(current_scene);
    } else {
        PRINT_DEBUG("ERROR: *no main scene set !*");
        engine->exit();
    }
}
