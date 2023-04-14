#include <ratr0/debug_utils.h>
#include <ratr0/data_types.h>
#include <ratr0/engine.h>
#include <ratr0/input.h>
#include <ratr0/timers.h>

// Amiga specific subsystem access
#include <ratr0/amiga/input.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mENGINE\033[0m", __VA_ARGS__)

#define TASK_PRIORITY (20)

static Ratr0Engine *engine;

void ratr0_amiga_engine_startup(Ratr0Engine *eng)
{
    engine = eng;
    // set high task priority so multitasking does not
    // grab too much CPU
    SetTaskPri(FindTask(NULL), TASK_PRIORITY);
}
