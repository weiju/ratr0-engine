/** @file blitter.h
 *
 * Amiga Blitter functionality module. These are low-level functions that
 * are private to the display system. Don't use any of these functions
 * from user code.
 */
#pragma once
#ifndef __RATR0_BLITTER_H__
#define __RATR0_BLITTER_H__
#include <ratr0/data_types.h>
#include <ratr0/engine.h>

/** \brief surface reference from amiga/display.h */
struct Ratr0Surface;
/** \brief tilesheet reference from tilesheet.h */
struct Ratr0TileSheet;

/**
 * Starts up the blitter module.
 *
 * @param engine pointer to Ratr0Engine object
 */
extern void ratr0_blitter_startup(Ratr0Engine *engine);


/******************************************************
 *
 * RECTANGULAR BLITS
 *
 ******************************************************/
/**
 * Common case 1: Blasting a rectangular block from the source to the destination
 * without masking or shifting, where the block width is a multiple of 16 and
 * the source and destination are arranged in multiples of 16 pixels.
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
extern UINT16 ratr0_blit_rect_simple(struct Ratr0Surface *dst,
                                     struct Ratr0Surface *src,
                                     UINT16 dstx, UINT16 dsty,
                                     UINT16 srcx, UINT16 srcy,
                                     UINT16 blit_width_pixels,
                                     UINT16 blit_height_pixels);

/**
 * Follow up blit to ratr0_blit_simple(). Because the blit is almost completely
 * set up, we can save some setup time by using most of the same register values.
 *
 * @param dst destination surface
 * @param src source surface
 * @param dstx destination x-coordinate
 * @param dsty destination y-coordinate
 * @param srcx source x-coordinate
 * @param srcy source y-coordinate
 * @param bltsize blit size
 */
extern void ratr0_blit_rect_simple2(struct Ratr0Surface *dst,
                                    struct Ratr0Surface *src,
                                    UINT16 dstx, UINT16 dsty,
                                    UINT16 srcx, UINT16 srcy,
                                    UINT16 bltsize);



/**
 * Common case 2: Copy an arbitrary sized rectangular block from source A to
 * an arbitrary location in the destination D, shifting might be applied
 depending on the parameters.
 *
 * Notes:
 *   * srcx should be on a 16 pixel boundary
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
extern UINT16 ratr0_blit_rect_ad(struct Ratr0Surface *dst,
                                 struct Ratr0Surface *src,
                                 UINT16 dstx, UINT16 dsty,
                                 UINT16 srcx, UINT16 srcy,
                                 UINT16 blit_width_pixels,
                                 UINT16 blit_height_pixels);

/**
 * Diagnostic blit function. Instead of performing an A->D blit,
 * this function simply prints out all the calculated values.
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
extern UINT16 ratr0_diag_blit_rect_ad(struct Ratr0Surface *dst,
                                   struct Ratr0Surface *src,
                                   UINT16 dstx, UINT16 dsty,
                                   UINT16 srcx, UINT16 srcy,
                                   UINT16 blit_width_pixels,
                                   UINT16 blit_height_pixels);

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
extern UINT16 ratr0_blit_8x8(struct Ratr0Surface *dst,
                             struct Ratr0Surface *font,
                             UINT16 dstx, UINT16 dsty,
                             char c,
                             UINT8 plane_num);


/**
 * Special case of ratr0_blit_rect. This can blit the specified tile from the source sheet
 * into the destination surface at the specified position. This is useful when we only want to
 * blit single planes and in rectangular shape, without a mask but with a shift. This can significantly
 * speed up blits of BOBs that only have a single color (like in Space invaders)
 *
 * @param dst destination surface
 * @param bobs the source tile sheet
 * @param tilex the x position of the tile
 * @param tiley the y position of the tile
 * @param dstx the x position in the destination surface
 * @param dsty the y position in the destination surface
 */
void ratr0_blit_rect_1plane(struct Ratr0Surface *dst,
                                  struct Ratr0TileSheet *bobs,
                                  int tilex, int tiley,
                                  int dstx, int dsty);

/******************************************************
 *
 * MASKED BLITS
 *
 ******************************************************/

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
extern void ratr0_blit_object(struct Ratr0Surface *dst,
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
extern void ratr0_blit_object_il(struct Ratr0Surface *dst,
                                 struct Ratr0TileSheet *bobs,
                                 int tilex, int tiley,
                                 int dstx, int dsty);

#endif /* __RATR0_BLITTER_H__ */
