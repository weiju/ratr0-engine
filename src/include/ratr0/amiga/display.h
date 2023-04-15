#pragma once
#ifndef __RATR0_AMIGA_DISPLAY_H__
#define __RATR0_AMIGA_DISPLAY_H__
#include <ratr0/data_types.h>
#include <ratr0/engine.h>

/* Amiga Display subsystem */
/*
  Amiga specific information about the display, used both for
  initialization and query information.
*/
struct Ratr0AmigaDisplayInfo {
    /* Width and height will be restricted to 320x200 (NTSC)
       and 320x256 (PAL) for now */
    UINT16 width, height;

    /* This can be a value between 1 and 5 */
    UINT8 depth;

    /* Display buffer */
    BOOL use_doublebuffer;

    /* Readonly section */
    /* This is the Amiga relevant part. We can't explicitly set
       PAL or NTSC since it is dependent on the machine, so it is query
       only */
    BOOL is_pal;
};

/**
 * Information about the current display in this object.
 * We can use this for both playfield hardware setup and blitter setup.
 * In the future we can use this for backbuffer and/or dual playfield
 * information.
 */
struct Ratr0AmigaRenderContext {
    UINT16 width, height, depth;
    BOOL is_interleaved;
    void *display_buffer;
};

enum BlitType {
    BLIT_BLOCK, BLIT_BOB
};
struct Ratr0AmigaBlitCommand {
    struct Ratr0AmigaRenderContext *dst;
    struct Ratr0AmigaRenderContext *src;
};

/**
 * Start up the display subsystem.
 */
extern void ratr0_amiga_display_startup(Ratr0Engine *, struct Ratr0AmigaDisplayInfo *);

/**
 * Shut down the display subsystem.
 */
extern void ratr0_amiga_display_shutdown(void);

/**
 * Wait for vertical blank
 */
extern void ratr0_amiga_wait_vblank(void);

/**
 * Process all queued up render commands.
 */
extern void ratr0_amiga_display_update(void);

/**
 * Quick access functions to the copper list.
 */
/**
 * Adjust the copper list to point to the specified display buffer with the
 * given specification.
 * The data in the display buffer is assumed to be interleaved.
 */
extern struct Ratr0AmigaRenderContext *ratr0_amiga_get_render_context(void);

extern void ratr0_amiga_set_palette(UINT16 *colors, UINT8 num_colors);
extern void ratr0_amiga_display_set_sprite(int sprite_num, UINT16 *data);

#endif /* __RATR0_AMIGA_DISPLAY_H__ */
