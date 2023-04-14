#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/scenes.h>
#include <ratr0/amiga/scenes.h>

#ifdef AMIGA
#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[33;1mSCENES\033[0m", __VA_ARGS__)
#else
#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[34mSCENES\033[0m", __VA_ARGS__)
#endif

/*
 * The scenes system is where games are implemented on a high level and tied together.
 * It builds on all the subsystems that deal with low-level aspects and integrates
 * with the scripting system.
 */
static struct Ratr0SceneSystem scene_system;
static struct Ratr0NodeFactory node_factory;
static Ratr0Engine *engine;
static struct Ratr0Node *current_scene;

static void ratr0_scenes_shutdown(void);
static void ratr0_scenes_update(void);
static void ratr0_scenes_set_current_scene(struct Ratr0Node *);

/**
 * Node factory
 */
static struct Ratr0Node nodes[10];
static UINT16 next_node = 0;
static struct Ratr0NodeFactory *ratr0_scenes_get_node_factory(void) { return &node_factory; }
static struct Ratr0Node *ratr0_scenes_create_node(void)
{
    struct Ratr0Node *result = &nodes[next_node++];
    return result;
}
static struct Ratr0AnimatedSprite2D *ratr0_nf_create_animated_sprite(struct Ratr0TileSheet *,
                                                                     UINT8 *, UINT8, BOOL);

struct Ratr0SceneSystem *ratr0_scenes_startup(Ratr0Engine *eng)
{
    engine = eng;
#ifdef AMIGA
    ratr0_amiga_scenes_startup(eng);
#endif
    scene_system.shutdown = &ratr0_scenes_shutdown;
    scene_system.update = &ratr0_scenes_update;
    scene_system.set_current_scene = &ratr0_scenes_set_current_scene;

    // Node factory
    node_factory.create_node = &ratr0_scenes_create_node;
    scene_system.get_node_factory = &ratr0_scenes_get_node_factory;

    PRINT_DEBUG("Startup finished.");
    return &scene_system;
}

static void ratr0_scenes_shutdown(void)
{
    PRINT_DEBUG("Shutdown finished.");
}

static int update_counter = 0;
static void ratr0_scenes_update(void)
{
    update_counter++;
    if (update_counter == 100) {
        PRINT_DEBUG("SCENES UPDATE to 100 !!!");
        update_counter = 0;
    }
}

static void ratr0_scenes_set_current_scene(struct Ratr0Node *node)
{
    current_scene = node;
}

static struct Ratr0AnimatedSprite2D *ratr0_nf_create_animated_sprite(struct Ratr0TileSheet *tilesheet,
                                                                     UINT8 *frame_indexes, UINT8 num_indexes,
                                                                     BOOL is_hw)
{
#ifdef AMIGA
    if (is_hw) {
        return (struct Ratr0AnimatedSprite2D *) ratr0_create_amiga_sprite(tilesheet, frame_indexes, num_indexes);
    }
#else
    // TODO
    return NULL;
#endif
}
