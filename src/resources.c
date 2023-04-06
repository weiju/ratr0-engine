#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/resources.h>

#ifdef AMIGA
#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[33;1mRESOURCES\033[0m", __VA_ARGS__)
#else
#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[34;1mRESOURCES\033[0m", __VA_ARGS__)
#endif

void ratr0_resources_startup(void)
{
    PRINT_DEBUG("Startup finished.");
}

void ratr0_resources_shutdown(void)
{
    PRINT_DEBUG("Shutdown finished.");
}
