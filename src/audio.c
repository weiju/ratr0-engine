/** @file audio.c */
#include <ratr0/debug_utils.h>
#include <ratr0/audio.h>
#include <ratr0/memory.h>
#include <graphics/gfxbase.h>
#include <hardware/custom.h>
#include "../../ptplayer/ptplayer.h"

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("AUDIO", __VA_ARGS__)

#define SAMPLE_PERIOD_NTSC (162)
#define SAMPLE_PERIOD_PAL  (161)

extern struct Custom custom;
extern struct GfxBase *GfxBase;

static struct Ratr0AudioSystem audio_system;
UINT16 hardware_replay_period;

void ratr0_audio_shutdown(void);

struct Ratr0AudioSystem *ratr0_audio_startup(void)
{
    audio_system.shutdown = &ratr0_audio_shutdown;

    BOOL is_pal = (((struct GfxBase *) GfxBase)->DisplayFlags & PAL) == PAL;
    mt_install_cia(&custom, NULL, is_pal);
    hardware_replay_period = is_pal ? SAMPLE_PERIOD_PAL : SAMPLE_PERIOD_NTSC;
    PRINT_DEBUG("Startup finished.");
    return &audio_system;
}

void ratr0_audio_shutdown(void)
{
    mt_remove_cia(&custom);
    PRINT_DEBUG("Shutdown finished.");
}

void ratr0_audio_play_sound(struct Ratr0AudioSample *sample, INT8 channel)
{
    void *sample_data = ratr0_memory_block_address(sample->h_data);
    struct SfxStructure sound_fx = {
        sample_data,
        sample->num_bytes / 2,
        hardware_replay_period,
        AUDIO_DEFAULT_SOUNDFX_VOLUME, channel,
        AUDIO_DEFAULT_SOUNDFX_PRIORITY
    };
    mt_playfx(&custom, &sound_fx);
}

#define AUDIO_DEFAULT_MOD_SAMPLES (NULL)
#define AUDIO_DEFAULT_MOD_START (0)

void ratr0_audio_play_mod(struct Ratr0AudioProtrackerMod *mod)
{
    void *mod_data = ratr0_memory_block_address(mod->h_data);
    mt_init(&custom, mod_data, AUDIO_DEFAULT_MOD_SAMPLES,
            AUDIO_DEFAULT_MOD_START);
    mt_Enable = 1;
}

void ratr0_audio_stop_mod(void)
{
    mt_end(&custom);
}

void ratr0_audio_set_master_volume(UINT8 volume)
{
    mt_mastervol(&custom, volume);
}

void ratr0_audio_pause_playback(void)
{
    mt_Enable = 0;
}

void ratr0_audio_resume_playback(void)
{
    mt_Enable = 1;
}

void ratr0_audio_toggle_playback(void)
{
    mt_Enable = mt_Enable == 1 ? 0 : 1;
}
