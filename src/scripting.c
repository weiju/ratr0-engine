#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/scripting.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[32;1mSCRIPTING\033[0m", __VA_ARGS__)

static struct Ratr0ScriptingSystem scripting_system;
static Ratr0Engine *engine;
void ratr0_scripting_shutdown(void);

struct Ratr0ScriptingSystem *ratr0_scripting_startup(Ratr0Engine *eng)
{
    engine = eng;
    scripting_system.shutdown = &ratr0_scripting_shutdown;

    PRINT_DEBUG("Startup finished.");
    return &scripting_system;
}

void ratr0_scripting_shutdown(void)
{
    PRINT_DEBUG("Shutdown finished.");
}
