#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/resources.h>

#ifdef AMIGA
#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[33;1mRESOURCES\033[0m", __VA_ARGS__)
#else
#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[34;1mRESOURCES\033[0m", __VA_ARGS__)
#endif

void ratr0_resources_shutdown(void);
static struct Ratr0ResourceSystem resource_system;

struct Ratr0ResourceSystem *ratr0_resources_startup(void)
{
    resource_system.shutdown = &ratr0_resources_shutdown;
    PRINT_DEBUG("Startup finished.");
    return &resource_system;
}

void ratr0_resources_shutdown(void)
{
    PRINT_DEBUG("Shutdown finished.");
}
