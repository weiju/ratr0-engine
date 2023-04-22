#include <ratr0/debug_utils.h>
#include <ratr0/engine.h>
#include <ratr0/world.h>
#include <ratr0/resources.h>

#ifdef AMIGA
extern void ratr0_amiga_engine_post_startup(void);
#endif

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mMAIN\033[0m", __VA_ARGS__)

int main(int argc, char **argv)
{
    Ratr0Engine *engine = ratr0_engine_startup();
#ifdef AMIGA
    // just a hack to get the background in
    ratr0_amiga_engine_post_startup();
#endif
    engine->game_loop();
    engine->shutdown();

    return 0;
}
