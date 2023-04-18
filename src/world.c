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

static void ratr0_world_update_node(struct Ratr0Node *cur)
{
    // 1. do an update action on the current node
    /*
      if (cur->class_id ==  BACKDROP) {
       // Render backdrop
        // We actually need a system to manage the integrity of the
        // backdrop, e.g. restoring dirty rectangles etc.
        struct Ratr0Backdrop *backdrop = (struct Ratr0Backdrop *) cur;
        if (!backdrop->was_drawn) {
            // Put it in the queue
            ratr0_amiga_enqueue_blit_fast(ratr0_amiga_get_display_surface(),
                                          &backdrop->surface,
                                          0, 0, 0, 0,
                                          backdrop->surface.width, backdrop->surface.height);
            PRINT_DEBUG("enqueue blit backdrop here");
            backdrop->was_drawn = TRUE;
        } else {
            // dirty rectangle algorithm to restore destroyed parts
            }
    }
    */
    // 2. call update on all children
    struct Ratr0Node *cur_child = cur->children;
    while (cur_child) {
        ratr0_world_update_node(cur_child);
        cur_child = cur_child->next;
    }
}

static void ratr0_world_node_add_child(struct Ratr0Node *parent, struct Ratr0Node *child)
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
static struct Ratr0Node nodes[10];
static UINT16 next_node = 0;
static struct Ratr0Backdrop backdrops[2]; // we don't have many of those
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
    struct Ratr0Node *result = &nodes[next_node++];
    ratr0_world_init_base_node(result, RATR0_NODE);
    return result;
}

static struct Ratr0AnimatedSprite *ratr0_nf_create_animated_sprite(struct Ratr0TileSheet *,
                                                                   UINT8 *, UINT8, BOOL);

static struct Ratr0Backdrop *ratr0_nf_create_backdrop(struct Ratr0TileSheet *tilesheet);


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

static struct Ratr0Backdrop *ratr0_nf_create_backdrop(struct Ratr0TileSheet *tilesheet)
{
    struct Ratr0Backdrop *result = &backdrops[next_backdrop++];
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
