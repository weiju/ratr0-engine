#include <ratr0/amiga/blitter.h>
#include <ratr0/amiga/display.h>
#include <hardware/custom.h>

extern struct Custom custom;
static Ratr0Engine *engine;


/**
 * Blit 8x8 tiles. Mostly used for fonts or small blocks. This should not be
 * the main blit function since widths multiples of 16 are way more efficient
 * on the Amiga.
 * This is a masked blit where 8 pixels are masked out, depending on
 * where the tile is
 */
void ratr0_amiga_blit_8x8(struct Ratr0AmigaRenderContext *dest,
                          struct Ratr0AmigaRenderContext *src,
                          UINT16 dstx, UINT16 dsty, UINT16 srcx, UINT16 srcy)
{
    /*
    int height = font.header.tile_height * font.header.bmdepth;
    // convert horizontal grid position to word position
    // we do this weird / 2, * 2 computation because dest needs to be even address
    int xword = (gridx >> 1) << 1;
    int dest_offset = BUFFER_ROW_BYTES * NUM_BITPLANES * gridy * 8 + xword;
    int frame_offset = height * 2 * ch;
    UBYTE *dst = dest + dest_offset;
    //UWORD a_mask = (gridx % 2 == 0) ? 0x00ff : 0xff00;
    UWORD a_mask = (gridx % 2 == 0) ? 0xff00 : 0x00ff;

    WaitBlit();
    custom.bltcon0 = 0x7ca;  // cookie cut, a is mask (from DAT), b is source, c is background, d is background
    custom.bltcon1 = 0;            // copy direction: ascending
    custom.bltbpt = font.imgdata + frame_offset;
    custom.bltcpt = dst;
    custom.bltdpt = dst;
    custom.bltbmod = 0;
    custom.bltcmod = 28;
    custom.bltdmod = 28;  // buffer width - 1 word (blit width)
    // copy everything, bltafwm and bltalwm are all set to 1's
    custom.bltafwm = 0xffff;
    custom.bltalwm = 0xffff;

    // DMA A is disabled, use as mask
    custom.bltadat = a_mask;  // set it to the half that we mask
    custom.bltcdat = 0xffff;
    custom.bltsize = (UWORD) (height << 6) | 1;  // the width will always be 1 word
    */
}

/**
 * Default fast 16x16 tile blit, D = A. This is the fastest graphical blit
 * and should be preferred when possible.
 *
 * There are some assumptions to keep this simple:
 *   - src is aligned on a 16 bit boundary and both width and height are
 *     multiples of 16
 *   - dst is aligned on a 16 bit boundary and is larger than the blit width
 *   - both src and dst have the same depth
 */
void ratr0_amiga_blit_fast(struct Ratr0AmigaRenderContext *dst,
                           struct Ratr0AmigaRenderContext *src,
                           UINT16 dstx, UINT16 dsty, UINT16 srcx, UINT16 srcy,
                           UINT16 blit_width_pixels, UINT16 blit_height_pixels)
{
    INT8 dst_shift = 0;
    UINT16 blit_width_words = blit_width_pixels / 16;  // blit width in words
    custom.bltafwm = 0xffff;
    //custom.bltalwm = alwm;

    // D = A => LF = 0xf0, channels A and D turned on => 0x09
    custom.bltcon0 = 0x09f0 | (dst_shift << 12);
    // not used
    custom.bltcon1 = 0;

    // modulos are in *bytes*
    UINT16 srcmod = src->width / 8 - 2;
    UINT16 dstmod = dst->width / 8 - (blit_width_words * 2);
    custom.bltamod = srcmod;
    custom.bltbmod = 0;
    custom.bltcmod = 0;
    custom.bltdmod = dstmod;

    /*
    custom.bltapt = src_addr;
    custom.bltbpt = 0;
    custom.bltcpt = 0;
    custom.bltdpt = dst_addr;
    custom.bltsize = bltsize;
    */
}

/*
static void blit_object(struct Ratr0TileSheet *bobs,
			struct Ratr0TileSheet *background,
			int tilex, int tiley,
			int dstx, int dsty)
{
    // actual object width (without the padding)
    int tile_width_pixels = bobs->header.tile_width - SHIFT_PADDING;

    // this tile's x-position relative to the word containing it
    int tile_x0 = bobs->header.tile_width * tilex & 0x0f;

    // 1. determine how wide the blit actually is
    int blit_width = tile_width_pixels / 16;

    // width not a multiple of 16 ? -> add 1 to the width
    if (tile_width_pixels & 0x0f) blit_width++;

    int blit_width0_pixels = blit_width * 16;  // blit width in pixels

    // Final source blit width: does the tile extend into an additional word ?
    int src_blit_width = blit_width;
    if (tile_x0 > blit_width0_pixels - tile_width_pixels) src_blit_width++;

    // 2. Determine the amount of shift and the first word in the
    // destination
    int dst_x0 = dstx & 0x0f;  // destination x relative to the containing word
    int dst_shift = dst_x0 - tile_x0;  // shift amount
    int dst_blit_width = blit_width;
    int dst_offset = 0;

    // negative shift => shift is to the left, so we extend the shift to the
    // left and right-shift in the previous word so we always right-shift
    if (dst_shift < 0) {
        dst_shift = 16 + dst_shift;
        dst_blit_width++;
        dst_offset = -2;
    }

    // make the blit wider if it needs more space
    if (dst_x0 > blit_width0_pixels - tile_width_pixels) {
        dst_blit_width++;
    }

    UWORD alwm = 0xffff;
    int final_blit_width = src_blit_width;

    // due to relative positioning and shifts, the destination blit width
    // can be larger than the source blit, so we use the larger of the 2
    // and mask out last word of the source
    if (dst_blit_width > src_blit_width) {
        final_blit_width = dst_blit_width;
        alwm = 0;
    }

    WaitBlit();

    custom.bltafwm = 0xffff;
    custom.bltalwm = alwm;

    // cookie cut enable channels B, C and D, LF => D = AB + ~AC => 0xca
    // A = Mask sheet
    // B = Tile sheet
    // C = Background
    // D = Background
    custom.bltcon0 = 0x0fca | (dst_shift << 12);
    custom.bltcon1 = dst_shift << 12;  // shift in B

    // modulos are in bytes
    UWORD srcmod = bobs->header.width / 8 - (final_blit_width * 2);
    UWORD dstmod = background->header.width / 8 - (final_blit_width * 2);
    custom.bltamod = srcmod;
    custom.bltbmod = srcmod;
    custom.bltcmod = dstmod;
    custom.bltdmod = dstmod;

    // The blit size is the size of a plane of the tile size (1 word * 16)
    UWORD bltsize = ((bobs->header.tile_height) << 6) |
        (final_blit_width & 0x3f);

    // map the tile position to physical coordinates in the tile sheet
    int srcx = tilex * bobs->header.tile_width;
    int srcy = tiley * bobs->header.tile_height;

    int bobs_plane_size = bobs->header.width / 8 * bobs->header.height;
    int bg_plane_size = background->header.width / 8 * background->header.height;

    UBYTE *src = bobs->imgdata + srcy * bobs->header.width / 8 + srcx / 8;
    // The mask data is the plane after the source image planes
    UBYTE *mask = bobs->imgdata + bobs_plane_size * bobs->header.bmdepth +
        srcy * bobs->header.width / 8 + srcx / 8;
    UBYTE *dst = background->imgdata + dsty * background->header.width / 8 +
        dstx / 8 + dst_offset;

    for (int i = 0; i < bobs->header.bmdepth; i++) {

        custom.bltapt = mask;
        custom.bltbpt = src;
        custom.bltcpt = dst;
        custom.bltdpt = dst;
        custom.bltsize = bltsize;

        // Increase the pointers to the next plane
        src += bobs_plane_size;
        dst += bg_plane_size;

        WaitBlit();
    }
}
*/
