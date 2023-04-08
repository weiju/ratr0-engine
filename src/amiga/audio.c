#include <ratr0/debug_utils.h>
#include <ratr0/amiga/audio.h>
#include <graphics/gfxbase.h>
#include <hardware/custom.h>
#include "../../ptplayer/ptplayer.h"

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[33;1mAUDIO\033[0m", __VA_ARGS__)

extern struct Custom custom;
extern struct GfxBase *GfxBase;

void ratr0_amiga_audio_startup(void)
{
    BOOL is_pal = (((struct GfxBase *) GfxBase)->DisplayFlags & PAL) == PAL;
    mt_install_cia(&custom, NULL, is_pal);
}

void ratr0_amiga_audio_shutdown(void)
{
    mt_remove_cia(&custom);
    PRINT_DEBUG("Shutdown finished.");
}
