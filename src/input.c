#include <ratr0/debug_utils.h>
#include <ratr0/input.h>

#ifdef AMIGA
#include <ratr0/amiga/input.h>
#endif /* AMIGA */

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[31;1mINPUT\033[0m", __VA_ARGS__)

void ratr0_input_startup(void)
{
#ifdef AMIGA
    ratr0_amiga_input_startup();
#endif
    PRINT_DEBUG("Startup finished.");
}

void ratr0_input_shutdown(void)
{
#ifdef AMIGA
    ratr0_amiga_input_shutdown();
#endif
    PRINT_DEBUG("Shutdown finished.");
}
