/** @file rendering.h
 *
 * Rendering subsystem
 */
#pragma once
#ifndef __RATR0_RENDERING_H__
#define __RATR0_RENDERING_H__
#include <ratr0/data_types.h>
#include <ratr0/engine.h>

/**
 * This data structure is used to configure the display.
 */
struct Ratr0DisplayInfo {
    /** \brief viewport width */
    UINT16 vp_width;
    /** \brief viewport height */
    UINT16 vp_height;

    /** \brief display buffer width */
    UINT16 buffer_width;
    /** \brief display buffer height */
    UINT16 buffer_height;
    /** \brief display buffer number of bitplanes */
    UINT8 depth;
    /** \brief display buffer number of buffers (should be 2) */
    UINT8 num_buffers;
};

/**
 * Surface is a rendering target, it is an abstract thing.
 */
struct Ratr0Surface;

/**
 * Interface to the rendering subsystem.
 */
struct Ratr0RenderingSystem {
    /**
     * Shuts down the rendering subsystem.
     */
    void (*shutdown)(void);
};

/**
 * Start up the display subsystem.
 *
 * @param engine pointer to engine instance
 * @param display_info configuration informatino for the display.
 * @return pointer to rendering subsystem object
 */
extern struct Ratr0RenderingSystem *ratr0_rendering_startup(Ratr0Engine *engine,
                                                            struct Ratr0DisplayInfo *display_info);

#endif /* __RATR0_RENDERING_H__ */
