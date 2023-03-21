#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/audio.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[34mAUDIO\033[0m", __VA_ARGS__)

extern void ratr0_audio_startup()
{
    PRINT_DEBUG("Start up...\n");
    PRINT_DEBUG("Startup finished.\n");
}

extern void ratr0_audio_shutdown()
{
    PRINT_DEBUG("Shutting down...\n");
    PRINT_DEBUG("Shutdown finished.\n");
}
