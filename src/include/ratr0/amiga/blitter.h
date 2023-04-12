#pragma once
#ifndef __RATR0_AMIGA_BLITTER_H__
#define __RATR0_AMIGA_BLITTER_H__
#include <ratr0/data_types.h>
#include <ratr0/engine.h>

/* Amiga Blitter functionality module */

struct Ratr0AmigaRenderContext;  // from display

extern void ratr0_amiga_blitter_startup(Ratr0Engine *eng);
extern void ratr0_amiga_blitter_shutdown(void);


/**
 * Blitter routines.
 */
/*
 * Common case 1: Blasting an 16x16 block from the source to the destination without masking.
 */
extern void ratr0_amiga_blit_16x16(struct Ratr0AmigaRenderContext *dest,
                                   struct Ratr0AmigaRenderContext *src,
                                   UINT16 dstx, UINT16 dsty, UINT16 srcx, UINT16 srcy);


/*
 * Common case 2: Blasting an 8x8 block from the source to the destination without masking.
 */
extern void ratr0_amiga_blit_8x8(struct Ratr0AmigaRenderContext *dest,
                                 struct Ratr0AmigaRenderContext *src,
                                 UINT16 dstx, UINT16 dsty, UINT16 srcx, UINT16 srcy);


#endif /* __RATR0_AMIGA_BLITTER_H__ */
