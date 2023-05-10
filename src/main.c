/** @file main.c */
#include <ratr0/debug_utils.h>
#include <ratr0/engine.h>
#include <ratr0/world.h>
#include <ratr0/resources.h>
#include <ratr0/main_scene.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mMAIN\033[0m", __VA_ARGS__)


int main(int argc, char **argv)
{
    Ratr0Engine *engine = ratr0_engine_startup();

    struct Ratr0Scene *main_scene = setup_main_scene(engine);
    engine->world_system->set_current_scene(main_scene);

    // START THE ENGINE !!!!
    engine->game_loop();
    engine->shutdown();

    return 0;
}
