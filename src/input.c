/** @file input.c */
#include <ratr0/debug_utils.h>
#include <ratr0/input.h>

#ifdef AMIGA
#include <ratr0/amiga/input.h>
#endif /* AMIGA */

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[31;1mINPUT\033[0m", __VA_ARGS__)

void ratr0_input_shutdown(void);
void ratr0_input_update(void);
static struct Ratr0InputSystem input_system;
static Ratr0Engine *engine;

struct Ratr0InputSystem *ratr0_input_startup(Ratr0Engine *eng)
{
    engine = eng;
#ifndef AMIGA
    input_system.shutdown = &ratr0_input_shutdown;
    input_system.update = &ratr0_input_update;
#else
    input_system.shutdown = &ratr0_amiga_input_shutdown;
    input_system.update = &ratr0_amiga_input_update;
    input_system.get_joystick_state = &ratr0_amiga_get_joystick_state;
    ratr0_amiga_input_startup();
#endif
    PRINT_DEBUG("Startup finished.");
    return &input_system;
}


#ifndef AMIGA
void ratr0_input_shutdown(void)
{
    PRINT_DEBUG("Shutdown finished.");
}

void ratr0_input_update(void)
{
    PRINT_DEBUG("Shutdown finished.");
}
#endif
