#include <ratr0/engine.h>


int main(int argc, char **argv)
{
    Ratr0Engine *engine = ratr0_engine_startup();
    // TODO: Use the scene system to setup the objects of the game

    // Then run the game loop
    engine->game_loop();
    engine->shutdown();

    return 0;
}
