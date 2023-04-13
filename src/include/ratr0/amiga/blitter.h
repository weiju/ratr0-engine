#pragma once
#ifndef __RATR0_AMIGA_BLITTER_H__
#define __RATR0_AMIGA_BLITTER_H__
#include <ratr0/data_types.h>
#include <ratr0/engine.h>

/* Amiga Blitter functionality module */

struct Ratr0AmigaRenderContext;  // from display
struct Ratr0TileSheet;

extern void ratr0_amiga_blitter_startup(Ratr0Engine *eng);
extern void ratr0_amiga_blitter_shutdown(void);


/**
 * Blitter routines.
 */
/*
 * Common case 1: Blasting a rectangular block from the source to the destination without masking,
 * where the block width is a multiple of 16 and the source is arranged in multiples of 16 pixels.
 */
extern void ratr0_amiga_blit_fast(struct Ratr0AmigaRenderContext *dest,
                                  struct Ratr0AmigaRenderContext *src,
                                  UINT16 dstx, UINT16 dsty, UINT16 srcx, UINT16 srcy,
                                  UINT16 blit_width_pixels, UINT16 blit_width_height);



/*
 * Common case 2: Blit a masked object to the screen. This is a general blit.
 */
extern void ratr0_amiga_blit_object(struct Ratr0AmigaRenderContext *dst,
                                    struct Ratr0TileSheet *bobs,
                                    int tilex, int tiley,
                                    int dstx, int dsty);

/*
 * Common case 3: Blasting an 8x8 block from the source to the destination without masking.
 */
extern void ratr0_amiga_blit_8x8(struct Ratr0AmigaRenderContext *dest,
                                 struct Ratr0AmigaRenderContext *src,
                                 UINT16 dstx, UINT16 dsty, UINT16 srcx, UINT16 srcy);


#endif /* __RATR0_AMIGA_BLITTER_H__ */
