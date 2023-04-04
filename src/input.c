#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/input.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[31;1mINPUT\033[0m", __VA_ARGS__)

void ratr0_input_startup(void)
{
    PRINT_DEBUG("Start up...");
    PRINT_DEBUG("Startup finished.");
}

void ratr0_input_shutdown(void)
{
    PRINT_DEBUG("Shutting down...");
    PRINT_DEBUG("Shutdown finished.");
}
