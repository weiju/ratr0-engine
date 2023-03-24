#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/audio.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[34mAUDIO\033[0m", __VA_ARGS__)

extern void ratr0_audio_startup()
{
    PRINT_DEBUG("Start up...");
    PRINT_DEBUG("Startup finished.");
}

extern void ratr0_audio_shutdown()
{
    PRINT_DEBUG("Shutting down...");
    PRINT_DEBUG("Shutdown finished.");
}
