/** @file engine.c */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <ratr0/debug_utils.h>
#include <ratr0/engine.h>
#include <ratr0/timers.h>
#include <ratr0/memory.h>
#include <ratr0/events.h>
#include <ratr0/audio.h>
#include <ratr0/display.h>
#include <ratr0/input.h>
#include <ratr0/resources.h>
#include <ratr0/scenes.h>

#define MAX_TIMERS (10)
#define TASK_PRIORITY (20)

#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>


#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[36mENGINE\033[0m", __VA_ARGS__)

enum { GAMESTATE_QUIT, GAMESTATE_RUNNING };
static int game_state = GAMESTATE_RUNNING;

void ratr0_engine_exit(void)
{
    game_state = GAMESTATE_QUIT;
}

static Ratr0Engine engine;
void ratr0_engine_shutdown(void);

static volatile UINT16 *custom_color00 = (volatile UINT16 *) 0xdff180;
void ratr0_engine_game_loop(void)
{
    while (game_state != GAMESTATE_QUIT) {
        WaitTOF();
        //*custom_color00 = 0xf00;
        // comment in for visual timing the loop iteration
        engine.scenes_system->update(frames_elapsed);
        //*custom_color00 = 0x000;
        // we are done with the back buffer. now swap it to the front
        ratr0_display_swap_buffers();
        frames_elapsed = 0;  // Reset the update frame counter
    }
}

Ratr0Engine *ratr0_engine_startup(struct Ratr0MemoryConfig *memory_config,
                                  struct Ratr0DisplayInfo *display_info)
{
    // hook in the shutdown function
    engine.shutdown = &ratr0_engine_shutdown;

    // Amiga specific startup code
    engine.game_loop = &ratr0_engine_game_loop;
    engine.exit = &ratr0_engine_exit;

    // set high task priority so multitasking does not
    // grab too much CPU
    SetTaskPri(FindTask(NULL), TASK_PRIORITY);

    PRINT_DEBUG("Start up...");

    // initialize the C random generator
    srand(time(NULL));
    engine.memory_system = ratr0_memory_startup(&engine, memory_config);
    //engine.event_system = ratr0_events_startup(&engine);
    engine.timer_system = ratr0_timers_startup(&engine, MAX_TIMERS);
    engine.input_system = ratr0_input_startup(&engine);
    engine.rendering_system = ratr0_display_startup(&engine, display_info);
    engine.audio_system = ratr0_audio_startup();
    engine.resource_system = ratr0_resources_startup(&engine);
    engine.scenes_system = ratr0_scenes_startup(&engine);
    PRINT_DEBUG("Startup finished.");
    return &engine;
}

void ratr0_engine_shutdown(void)
{
    PRINT_DEBUG("Shutting down...");
    engine.scenes_system->shutdown();
    engine.resource_system->shutdown();
    engine.audio_system->shutdown();
    engine.rendering_system->shutdown();
    engine.input_system->shutdown();
    engine.timer_system->shutdown();
    //engine.event_system->shutdown();
    engine.memory_system->shutdown();

    PRINT_DEBUG("Shutdown finished.");
}
