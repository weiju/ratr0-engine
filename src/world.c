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
 * Node factory
 */
static struct Ratr0Node nodes[10];
static UINT16 next_node = 0;
static struct Ratr0NodeFactory *ratr0_world_get_node_factory(void) { return &node_factory; }
static struct Ratr0Node *ratr0_world_create_node(void)
{
    struct Ratr0Node *result = &nodes[next_node++];
    return result;
}
static struct Ratr0AnimatedSprite *ratr0_nf_create_animated_sprite(struct Ratr0TileSheet *,
                                                                   UINT8 *, UINT8, BOOL);

struct Ratr0WorldSystem *ratr0_world_startup(Ratr0Engine *eng)
{
    engine = eng;
#ifdef AMIGA
    ratr0_amiga_world_startup(eng);
#endif
    world_system.shutdown = &ratr0_world_shutdown;
    world_system.update = &ratr0_world_update;
    world_system.set_current_scene = &ratr0_world_set_current_scene;

    // Node factory
    node_factory.create_node = &ratr0_world_create_node;
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

static void ratr0_world_update(void)
{
    // TODO: Push the objects in the scene to the rendering 
}
