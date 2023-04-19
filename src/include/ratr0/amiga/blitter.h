#pragma once
#ifndef __RATR0_AMIGA_BLITTER_H__
#define __RATR0_AMIGA_BLITTER_H__
#include <ratr0/data_types.h>
#include <ratr0/engine.h>

/* Amiga Blitter functionality module */
struct Ratr0AmigaSurface;  // from display
struct Ratr0TileSheet;

/**
 * Blitter commands are just data.
 */
enum BlitType {
    BLIT_BLOCK, BLIT_BOB
};

struct Ratr0AmigaBlitCommand {
    UINT16 blit_type;

    UINT32 dst_addr, src_addr, mask_addr;
    UINT16 dstmod, srcmod, bltsize;
    UINT16 dst_row_bytes, src_plane_size, alwm;
    UINT8 num_planes;
    INT8 dst_shift;
};


extern void ratr0_amiga_blitter_startup(Ratr0Engine *eng);
extern void ratr0_amiga_do_blit_command(struct Ratr0AmigaBlitCommand *cmd);

/**
 * Functions that initialize blit commands.
 */
/*
 * Common case 1: Blasting a rectangular block from the source to the destination without masking,
 * where the block width is a multiple of 16 and the source is arranged in multiples of 16 pixels.
 * Fast blit: Initialize a blitter command that is part of the queue.
 */
extern void ratr0_amiga_make_blit_fast(struct Ratr0AmigaBlitCommand *cmd,
                                       struct Ratr0AmigaSurface *dst,
                                       struct Ratr0AmigaSurface *src,
                                       UINT16 dstx, UINT16 dsty, UINT16 srcx, UINT16 srcy,
                                       UINT16 blit_width_pixels, UINT16 blit_height_pixels);


/*
 * Direct fast blit function.
 */
/*
extern void ratr0_amiga_blit_fast(struct Ratr0AmigaSurface *dst,
                                  struct Ratr0AmigaSurface *src,
                                  UINT16 dstx, UINT16 dsty, UINT16 srcx, UINT16 srcy,
                                  UINT16 blit_width_pixels, UINT16 blit_height_pixels);
*/
/*
 * Common case 2: Blit a masked object to the screen. This is a general blit.
 */
extern void ratr0_amiga_make_blit_object(struct Ratr0AmigaBlitCommand *cmd,
                                         struct Ratr0AmigaSurface *dst,
                                         struct Ratr0TileSheet *bobs,
                                         int tilex, int tiley,
                                         int dstx, int dsty);

/*
 * Common case 3: Blasting an 8x8 block from the source to the destination without masking.
 */
extern void ratr0_amiga_blit_8x8(struct Ratr0AmigaSurface *dst,
                                 struct Ratr0AmigaSurface *src,
                                 UINT16 dstx, UINT16 dsty, UINT16 srcx, UINT16 srcy);


#endif /* __RATR0_AMIGA_BLITTER_H__ */
