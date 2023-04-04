#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/audio.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[34mAUDIO\033[0m", __VA_ARGS__)

void ratr0_audio_startup(void)
{
    PRINT_DEBUG("Start up...");
    PRINT_DEBUG("Startup finished.");
}

void ratr0_audio_shutdown(void)
{
    PRINT_DEBUG("Shutting down...");
    PRINT_DEBUG("Shutdown finished.");
}
