#include <hardware/custom.h>
#include <clib/graphics_protos.h>

#include <ratr0/debug_utils.h>
#include <ratr0/resources.h>
#include <ratr0/amiga/blitter.h>
#include <ratr0/amiga/display.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[32mBLITTER\033[0m", __VA_ARGS__)

extern struct Custom custom;
static Ratr0Engine *engine;


void ratr0_amiga_blitter_startup(Ratr0Engine *eng)
{
    engine = eng;
}

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
    WaitBlit();
    custom.bltafwm = 0xffff;
    custom.bltalwm = 0xffff;

    // D = A => LF = 0xf0, channels A and D turned on => 0x09
    custom.bltcon0 = 0x09f0 | (dst_shift << 12);
    // not used
    custom.bltcon1 = 0;

    // modulos are in *bytes*
    UINT16 srcmod = src->width / 8 - (blit_width_words * 2);
    UINT16 dstmod = dst->width / 8 - (blit_width_words * 2);
    custom.bltamod = srcmod;
    custom.bltbmod = 0;
    custom.bltcmod = 0;
    custom.bltdmod = dstmod;

    UINT32 src_addr = ((UINT32) src->display_buffer) + (src->width / 8 * srcy * src->depth) + srcx / 8;
    UINT32 dst_addr = ((UINT32) dst->display_buffer) + (dst->width / 8 * dsty * dst->depth) + dstx / 8;

    custom.bltapt = (UINT8 *) src_addr;
    custom.bltbpt = 0;
    custom.bltcpt = 0;
    custom.bltdpt = (UINT8 *) dst_addr;
    UINT16 bltsize = (UINT16) ((blit_height_pixels * src->depth) << 6) | (blit_width_words & 0x3f);
    custom.bltsize = bltsize;
}

/**
 * This should be rare, but serves as an illustration for when the
 * source is non-interleaved and the destination is interleaved.
 * The modulo for the destination needs to be (row bytes * number of planes) - blit width in bytes
 */
void ratr0_amiga_blit_ni(struct Ratr0AmigaRenderContext *dst,
                         struct Ratr0AmigaRenderContext *src,
                         UINT16 dstx, UINT16 dsty, UINT16 srcx, UINT16 srcy,
                         UINT16 blit_width_pixels, UINT16 blit_height_pixels)
{
    INT8 dst_shift = 0;
    UINT16 blit_width_words = blit_width_pixels / 16;  // blit width in words
    UINT32 src_plane_size = src->width / 8 * src->height;
    UINT32 dst_row_bytes = dst->width / 8;
    WaitBlit();
    custom.bltafwm = 0xffff;
    custom.bltalwm = 0xffff;

    // D = A => LF = 0xf0, channels A and D turned on => 0x09
    custom.bltcon0 = 0x09f0 | (dst_shift << 12);
    // not used
    custom.bltcon1 = 0;

    // modulos are in *bytes*
    UINT16 srcmod = src->width / 8 - (blit_width_words * 2);
    UINT16 dstmod = dst_row_bytes * dst->depth - (blit_width_words * 2);
    custom.bltamod = srcmod;
    custom.bltbmod = 0;
    custom.bltcmod = 0;
    custom.bltdmod = dstmod;

    UINT32 src_addr = ((UINT32) src->display_buffer) + (src->width / 8 * srcy * src->depth) + srcx / 8;
    UINT32 dst_addr = ((UINT32) dst->display_buffer) + (dst->width / 8 * dsty * dst->depth) + dstx / 8;
    UINT16 bltsize = (UINT16) (blit_height_pixels << 6) | (blit_width_words & 0x3f);

    custom.bltapt = (UINT8 *) src_addr;
    custom.bltbpt = 0;
    custom.bltcpt = 0;
    custom.bltdpt = (UINT8 *) dst_addr;
    custom.bltsize = bltsize;
    WaitBlit();

    for (int i = 0; i < src->depth; i++) {
        custom.bltapt = (UINT8 *) src_addr;
        custom.bltbpt = 0;
        custom.bltcpt = 0;
        custom.bltdpt = (UINT8 *) dst_addr;
        UINT16 bltsize = (UINT16) (blit_height_pixels << 6) | (blit_width_words & 0x3f);
        custom.bltsize = bltsize;
        WaitBlit();
        src_addr += src_plane_size;
        dst_addr += dst_row_bytes;
    }
}

/**
 * This is a general purpose blit, cookie cut of arbitrary width,
 * with shifts.
 * We assume some simplifications for the cookie cutter blit:
 * - each BOB starts on a 16 pixel boundary
 * - There is at least a 16 pixel border at the right of each BOB,
 *   when there are multiple BOBs per row, they must be separated by
 *   at least 16 pixels
 * - Each tile has the width of a multiple of 16 pixels
 */
void ratr0_amiga_blit_object(struct Ratr0AmigaRenderContext *dst,
                             struct Ratr0TileSheet *bobs,
                             int tilex, int tiley,
                             int dstx, int dsty)
{
    // Source variables
    UINT16 src_blit_width_pixels = bobs->header.tile_width;
    UINT16 src_blit_width = src_blit_width_pixels / 16;
    UINT16 blit_height_pixels = bobs->header.tile_height;
    UINT16 srcx = tilex * bobs->header.tile_width;
    UINT16 srcy = tiley * bobs->header.tile_height;

    // Destination variables
    // destination x relative to the containing word, this is also the
    // the shift amount
    UINT8 dst_x0 = dstx & 0x0f;
    INT8 dst_shift = dst_x0;
    UINT16 dst_blit_width = src_blit_width;
    INT8 dst_offset = 0;

    // negative shift => shift is to the left, so we extend the shift to the
    // left and right-shift in the previous word so we always right-shift
    if (dst_shift < 0) {
        dst_shift = 16 + dst_shift;
        dst_blit_width++;
        dst_offset = -2;
    }

    UINT16 alwm = 0xffff;
    UINT16 final_blit_width = src_blit_width;
    if (dst_blit_width > src_blit_width) {
        final_blit_width = dst_blit_width;
        alwm = 0;
    }

    UINT32 src_plane_size = bobs->header.width / 8 * bobs->header.height;
    UINT32 dst_row_bytes = dst->width / 8;
    WaitBlit();
    custom.bltafwm = 0xffff;
    custom.bltalwm = alwm;

    // channels A-D turned on => 0x09 LF => D = AB + ~AC => 0xca
    custom.bltcon0 = 0x0fca | (dst_shift << 12);
    // used
    custom.bltcon1 = dst_shift << 12;

    // modulos are in *bytes*
    UINT16 srcmod = bobs->header.width / 8 - (final_blit_width * 2);
    UINT16 dstmod = dst_row_bytes * dst->depth - (final_blit_width * 2);
    custom.bltamod = srcmod;
    custom.bltbmod = srcmod;
    custom.bltcmod = dstmod;
    custom.bltdmod = dstmod;

    int bobs_plane_size = bobs->header.width / 8 * bobs->header.height;
    UINT8 *bobs_addr = engine->memory_system->block_address(bobs->h_imgdata);
    UINT8 *mask_addr = bobs_addr + bobs_plane_size * bobs->header.bmdepth +
        srcy * bobs->header.width / 8 + srcx / 8;
    UINT32 src_addr = ((UINT32) bobs_addr) + (bobs->header.width / 8 * srcy * bobs->header.bmdepth) + srcx / 8;
    UINT32 dst_addr = ((UINT32) dst->display_buffer) + (dst->width / 8 * dsty * dst->depth) + dstx / 8;
    UINT16 bltsize = (UINT16) (blit_height_pixels << 6) | (final_blit_width & 0x3f);

    for (int i = 0; i < bobs->header.bmdepth; i++) {
        custom.bltapt = (UINT8 *) mask_addr;
        custom.bltbpt = (UINT8 *) src_addr;
        custom.bltcpt = (UINT8 *) dst_addr;
        custom.bltdpt = (UINT8 *) dst_addr;
        UINT16 bltsize = (UINT16) (blit_height_pixels << 6) | (final_blit_width & 0x3f);
        custom.bltsize = bltsize;
        WaitBlit();
        src_addr += src_plane_size;
        dst_addr += dst_row_bytes;
    }
}
