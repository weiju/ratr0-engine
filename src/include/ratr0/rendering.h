#pragma once
#ifndef __RATR0_RENDERING_H__
#define __RATR0_RENDERING_H__
#include <ratr0/data_types.h>
#include <ratr0/engine.h>

/* Rendering subsystem */
struct Ratr0DisplayInfo {
    /* Size of viewport */
    UINT16 vp_width, vp_height;

    /* Size of display buffer */
    UINT16 buffer_width, buffer_height;
    UINT8 depth;
    UINT8 num_buffers;
};

/**
 * Surface is a rendering target, it is an abstract thing.
 */
struct Ratr0Surface;

struct Ratr0RenderingSystem {
    void (*shutdown)(void);
    void (*update)(void);
    void (*wait_vblank)(void);
    void (*enqueue_blit)(struct Ratr0Surface *ctx, UINT16 dstx, UINT16 dsty);
};

/**
 * Start up the display subsystem.
 */
extern struct Ratr0RenderingSystem *ratr0_rendering_startup(Ratr0Engine *, struct Ratr0DisplayInfo *);

#endif /* __RATR0_RENDERING_H__ */
