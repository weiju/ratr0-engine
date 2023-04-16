#pragma once
#ifndef __RATR0_DISPLAY_H__
#define __RATR0_DISPLAY_H__
#include <ratr0/data_types.h>
#include <ratr0/engine.h>

/* Display subsystem */
struct Ratr0DisplayInfo {
    UINT16 width, height;
    UINT8 depth;
    BOOL use_doublebuffer;
};

/**
 * Surface is a rendering target, it is an abstract thing.
 */
struct Ratr0Surface;

struct Ratr0DisplaySystem {
    void (*shutdown)(void);
    void (*update)(void);
    void (*wait_vblank)(void);
    void (*enqueue_blit)(struct Ratr0Surface *ctx, UINT16 dstx, UINT16 dsty);
};

/**
 * Start up the display subsystem.
 */
extern struct Ratr0DisplaySystem *ratr0_display_startup(Ratr0Engine *, struct Ratr0DisplayInfo *);

#endif /* __RATR0_DISPLAY_H__ */
