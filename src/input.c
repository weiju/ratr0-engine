#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/input.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[31;1mINPUT\033[0m", __VA_ARGS__)

extern void ratr0_input_startup()
{
    PRINT_DEBUG("Start up...");
    PRINT_DEBUG("Startup finished.");
}

extern void ratr0_input_shutdown()
{
    PRINT_DEBUG("Shutting down...");
    PRINT_DEBUG("Shutdown finished.");
}
