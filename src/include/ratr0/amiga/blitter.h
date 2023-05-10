/** @file blitter.h
 *
 * Amiga Blitter functionality module.
 */
#pragma once
#ifndef __RATR0_AMIGA_BLITTER_H__
#define __RATR0_AMIGA_BLITTER_H__
#include <ratr0/data_types.h>
#include <ratr0/engine.h>

/** \brief surface reference from amiga/display.h */
struct Ratr0AmigaSurface;
/** \brief tilesheet reference from tilesheet.h */
struct Ratr0TileSheet;

/**
 * Starts up the blitter module.
 *
 * @param engine pointer to Ratr0Engine object
 */
extern void ratr0_amiga_blitter_startup(Ratr0Engine *engine);

/**
 * Common case 1: Blasting a rectangular block from the source to the destination without masking,
 * where the block width is a multiple of 16 and the source is arranged in multiples of 16 pixels.
 *
 * @param dst destination surface
 * @param src source surface
 * @param dstx destination x-coordinate
 * @param dsty destination y-coordinate
 * @param srcx source x-coordinate
 * @param srcy source y-coordinate
 * @param blit_width_pixels blit width in pixels
 * @param blit_height_pixels blit height in pixels
 * @return blit size
 */
extern UINT16 ratr0_amiga_blit_rect(struct Ratr0AmigaSurface *dst,
                                    struct Ratr0AmigaSurface *src,
                                    UINT16 dstx, UINT16 dsty, UINT16 srcx, UINT16 srcy,
                                    UINT16 blit_width_pixels, UINT16 blit_height_pixels);

/**
 * Follow up blit to _blit_rect. Because the blit is almost completely set up,
 * we can save some setup time by using most of the same register values.
 *
 * @param dst destination surface
 * @param src source surface
 * @param dstx destination x-coordinate
 * @param dsty destination y-coordinate
 * @param srcx source x-coordinate
 * @param srcy source y-coordinate
 * @param bltsize blit size
 */
extern void ratr0_amiga_blit_rect_fast(struct Ratr0AmigaSurface *dst,
                                       struct Ratr0AmigaSurface *src,
                                       UINT16 dstx, UINT16 dsty, UINT16 srcx, UINT16 srcy,
                                       UINT16 bltsize);

/**
 * Common case 2: Blit a masked object to the screen. This is a general blit.
 *
 * @param dst destination surface
 * @param bobs source tilesheet
 * @param tilex tile x-coordinate
 * @param tiley tile y-coordinate
 * @param dstx destination x-coordinate
 * @param dsty destination y-coordinate
 */
extern void ratr0_amiga_blit_object(struct Ratr0AmigaSurface *dst,
                                    struct Ratr0TileSheet *bobs,
                                    int tilex, int tiley,
                                    int dstx, int dsty);

/**
 * Same as above, but with interleaved source to interleaved destination.
 *
 * @param dst destination surface
 * @param bobs source tilesheet
 * @param tilex tile x-coordinate
 * @param tiley tile y-coordinate
 * @param dstx destination x-coordinate
 * @param dsty destination y-coordinate
 */
extern void ratr0_amiga_blit_object_il(struct Ratr0AmigaSurface *dst,
                                       struct Ratr0TileSheet *bobs,
                                       int tilex, int tiley,
                                       int dstx, int dsty);

/**
 * Default font blitting function.
 *
 * @param dst destination surface
 * @param font font sheet
 * @param dstx destination x-coordinate
 * @param dsty destination y-coordinate
 * @param c character to blit
 * @param plane_num destination plane to blit into
 */
extern UINT16 ratr0_amiga_blit_8x8(struct Ratr0AmigaSurface *dst,
                                   struct Ratr0AmigaSurface *font,
                                   UINT16 dstx, UINT16 dsty,
                                   char c,
                                   UINT8 plane_num);

#endif /* __RATR0_AMIGA_BLITTER_H__ */
