#pragma once
#ifndef __RATR0_AMIGA_BLITTER_H__
#define __RATR0_AMIGA_BLITTER_H__
#include <ratr0/data_types.h>
#include <ratr0/engine.h>

/* Amiga Blitter functionality module */
struct Ratr0AmigaSurface;  // from display
struct Ratr0TileSheet;


extern void ratr0_amiga_blitter_startup(Ratr0Engine *eng);

/**
 * Common case 1: Blasting a rectangular block from the source to the destination without masking,
 * where the block width is a multiple of 16 and the source is arranged in multiples of 16 pixels.
 */
extern UINT16 ratr0_amiga_blit_rect(struct Ratr0AmigaSurface *dst,
                                    struct Ratr0AmigaSurface *src,
                                    UINT16 dstx, UINT16 dsty, UINT16 srcx, UINT16 srcy,
                                    UINT16 blit_width_pixels, UINT16 blit_height_pixels);

/**
 * Follow up blit to _blit_rect. Because the blit is almost completely set up,
 * we can save some setup time by using most of the same register values.
 */
extern void ratr0_amiga_blit_rect_fast(struct Ratr0AmigaSurface *dst,
                                       struct Ratr0AmigaSurface *src,
                                       UINT16 dstx, UINT16 dsty, UINT16 srcx, UINT16 srcy,
                                       UINT16 bltsize);

/*
 * Common case 2: Blit a masked object to the screen. This is a general blit.
 */
extern void ratr0_amiga_blit_object(struct Ratr0AmigaSurface *dst,
                                    struct Ratr0TileSheet *bobs,
                                    int tilex, int tiley,
                                    int dstx, int dsty);

/*
 * Same as above, but with interleaved source to interleaved destination.
 */
extern void ratr0_amiga_blit_object_il(struct Ratr0AmigaSurface *dst,
                                       struct Ratr0TileSheet *bobs,
                                       int tilex, int tiley,
                                       int dstx, int dsty);

/*
 * Default font blitting function.
 */
extern UINT16 ratr0_amiga_blit_8x8(struct Ratr0AmigaSurface *dst,
                                   struct Ratr0AmigaSurface *src,
                                   UINT16 dstx, UINT16 dsty,
                                   char c,
                                   UINT8 plane_num);

#endif /* __RATR0_AMIGA_BLITTER_H__ */
