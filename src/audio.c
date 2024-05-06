/** @file audio.c */
#include <ratr0/debug_utils.h>
#include <ratr0/audio.h>
#include <graphics/gfxbase.h>
#include <hardware/custom.h>
#include "../../ptplayer/ptplayer.h"

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[33;1mAUDIO\033[0m", __VA_ARGS__)

extern struct Custom custom;
extern struct GfxBase *GfxBase;

static struct Ratr0AudioSystem audio_system;
void ratr0_audio_shutdown(void);

struct Ratr0AudioSystem *ratr0_audio_startup(void)
{
    audio_system.shutdown = &ratr0_audio_shutdown;

    BOOL is_pal = (((struct GfxBase *) GfxBase)->DisplayFlags & PAL) == PAL;
    mt_install_cia(&custom, NULL, is_pal);
    PRINT_DEBUG("Startup finished.");
    return &audio_system;
}

void ratr0_audio_shutdown(void)
{
    mt_remove_cia(&custom);
    PRINT_DEBUG("Shutdown finished.");
}
