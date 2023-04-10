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

    /* Readonly section */
    /* This is the Amiga relevant part. We can't explicitly set
       PAL or NTSC since it is dependent on the machine, so it is query
       only */
    BOOL is_pal;
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
 * Experimental only
 */
extern void ratr0_amiga_display_set_sprite(int sprite_num, UINT16 *data);

#endif /* __RATR0_AMIGA_DISPLAY_H__ */
