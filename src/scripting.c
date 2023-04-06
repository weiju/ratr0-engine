#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/scripting.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[32;1mSCRIPTING\033[0m", __VA_ARGS__)

void ratr0_scripting_startup(void)
{
    PRINT_DEBUG("Startup finished.");
}

void ratr0_scripting_shutdown(void)
{
    PRINT_DEBUG("Shutdown finished.");
}
