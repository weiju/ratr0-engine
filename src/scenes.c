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
static void ratr0_scenes_shutdown(void);
static void ratr0_scenes_update(void);
static void ratr0_scenes_set_current_scene(struct Ratr0Node *);
static struct Ratr0NodeFactory *ratr0_scenes_get_node_factory(void) { return &node_factory; }
static struct Ratr0AnimatedSprite2D *ratr0_nf_create_animated_sprite(struct Ratr0TileSheet *,
                                                                     UINT8 *, BOOL);

struct Ratr0SceneSystem *ratr0_scenes_startup(Ratr0Engine *eng)
{
    engine = eng;
    ratr0_amiga_scenes_startup(eng);

    scene_system.shutdown = &ratr0_scenes_shutdown;
    scene_system.update = &ratr0_scenes_update;
    scene_system.set_current_scene = &ratr0_scenes_set_current_scene;

    // Node factory
    scene_system.get_node_factory = &ratr0_scenes_get_node_factory;

    PRINT_DEBUG("Startup finished.");
    return &scene_system;
}

static void ratr0_scenes_shutdown(void)
{
    PRINT_DEBUG("Shutdown finished.");
}

static void ratr0_scenes_update(void)
{
}

static void ratr0_scenes_set_current_scene(struct Ratr0Node *)
{
}

static struct Ratr0AnimatedSprite2D *ratr0_nf_create_animated_sprite(struct Ratr0TileSheet *tilesheet,
                                                                     UINT8 *frame_indexes, BOOL is_hw)
{
    return (struct Ratr0AnimatedSprite2D *) ratr0_create_amiga_sprite(tilesheet, frame_indexes);
}
