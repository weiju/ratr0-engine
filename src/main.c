#include <ratr0/engine.h>


int main(int argc, char **argv)
{
    Ratr0Engine *engine = ratr0_engine_startup();
    engine->game_loop();
    engine->shutdown();

    return 0;
}
