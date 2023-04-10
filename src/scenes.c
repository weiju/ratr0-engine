#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/scenes.h>

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

struct Ratr0SceneSystem *ratr0_scenes_startup(Ratr0Engine *eng)
{
    engine = eng;
    scene_system.shutdown = &ratr0_scenes_shutdown;
    scene_system.update = &ratr0_scenes_update;
    scene_system.set_current_scene = &ratr0_scenes_set_current_scene;
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
