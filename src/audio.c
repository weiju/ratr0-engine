#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/audio.h>
#include <ratr0/amiga/audio.h>

#ifdef AMIGA
#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[33;1mAUDIO\033[0m", __VA_ARGS__)
#else
#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[34mAUDIO\033[0m", __VA_ARGS__)
#endif

static struct Ratr0AudioSystem audio_system;
void ratr0_audio_shutdown(void);
static Ratr0Engine *engine;

struct Ratr0AudioSystem *ratr0_audio_startup(Ratr0Engine *eng)
{
    engine = eng;
    audio_system.shutdown = &ratr0_amiga_audio_shutdown;
    ratr0_amiga_audio_startup();
    PRINT_DEBUG("Startup finished.");
    return &audio_system;
}

#ifndef AMIGA
void ratr0_audio_shutdown(void)
{
    PRINT_DEBUG("Shutdown finished.");
}
#endif
