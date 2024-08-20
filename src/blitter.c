/** @file blitter.c */
#include <hardware/custom.h>
#include <clib/graphics_protos.h>

#include <ratr0/debug_utils.h>
#include <ratr0/resources.h>
#include <ratr0/blitter.h>
#include <ratr0/display.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[32mBLITTER\033[0m", __VA_ARGS__)

extern struct Custom custom;
static Ratr0Engine *engine;


void _blit_object(UINT32 dst_addr, UINT32 src_addr, UINT32 mask_addr,
                  UINT16 dstmod, UINT16 srcmod,
                  UINT8 num_planes,
                  UINT16 dst_row_bytes, UINT16 src_plane_size,
                  INT8 dst_shift, UINT16 alwm, UINT16 bltsize);

void _blit_object_il(UINT32 dst_addr, UINT32 src_addr, UINT32 mask_addr,
                     UINT16 dstmod, UINT16 srcmod,
                     INT8 dst_shift, UINT16 alwm, UINT16 bltsize);

void ratr0_blitter_startup(Ratr0Engine *eng)
{
    engine = eng;
}

/**
 * Default rectangular tile blit, D = A. This is the fastest graphical blit
 * and should be preferred when possible.
 *
 * There are some assumptions to keep this simple:
 *   - src is aligned on a 16 bit boundary and both width and height are
 *     multiples of 16
 *   - dst is aligned on a 16 bit boundary and is larger than the blit width
 *   - both src and dst have the same depth
 */
UINT16 _blit_rect_simple(UINT32 dst_addr, UINT32 src_addr, UINT16 dstmod,
                         UINT16 srcmod,
                         UINT16 bltsize)
{
    WaitBlit();
    // D = A => LF = 0xf0, channels A and D turned on => 0x09
    custom.bltcon0 = 0x09f0;
    // not used
    custom.bltcon1 = 0;

    custom.bltafwm = 0xffff;
    custom.bltalwm = 0xffff;

    custom.bltamod = srcmod;
    custom.bltbmod = 0;
    custom.bltcmod = 0;
    custom.bltdmod = dstmod;

    custom.bltapt = (UINT8 *) src_addr;
    custom.bltbpt = 0;
    custom.bltcpt = 0;
    custom.bltdpt = (UINT8 *) dst_addr;
    custom.bltsize = bltsize;
    return bltsize;
}

/**
 * Blit a rectangle after having blit a rectangle with the same source and
 * destination. In this case we only have to specify the source and
 * and destination address and set the blit size only for starting the blit
 */
void _blit_rect_simple2(UINT32 dst_addr, UINT32 src_addr, UINT16 bltsize)
{
    WaitBlit();
    custom.bltapt = (UINT8 *) src_addr;
    custom.bltdpt = (UINT8 *) dst_addr;
    custom.bltsize = bltsize;
}

UINT16 ratr0_blit_rect_simple(struct Ratr0Surface *dst,
                              struct Ratr0Surface *src,
                              UINT16 dstx, UINT16 dsty, UINT16 srcx, UINT16 srcy,
                              UINT16 blit_width_pixels, UINT16 blit_height_pixels)
{
    UINT16 blit_width_words = blit_width_pixels >> 4;  // blit width in words
    UINT16 src_width_bytes = src->width >> 3;
    UINT16 dst_width_bytes = dst->width >> 3;
    // modulos are in *bytes*
    UINT16 srcmod = src_width_bytes - (blit_width_words << 1);
    UINT16 dstmod = dst_width_bytes - (blit_width_words << 1);
    UINT32 src_addr = ((UINT32) src->buffer) + (src_width_bytes * srcy * src->depth) + (srcx >> 3);
    UINT32 dst_addr = ((UINT32) dst->buffer) + (dst_width_bytes * dsty * dst->depth) + (dstx >> 3);

    int blit_height_total = blit_height_pixels * src->depth;
    if (blit_height_total > 1024) {
        // EDGE CASE: since we are supporting OCS blitter, we might run into issues
        // with the blit size maximums, which is 1024x1024 pixels.
        // For now, I just assume blits will never exceed 1024 pixels (or 64 16-bit words) in width
        // 1. check if we exceed 1024 lines, which we can easily run into when blitting
        // interleaved screens with >= 5 bitplanes depth.
        // 2. I solve this by performing 2 blits, one of 1024 height, and the rest
        // 3. I assume that the modulos for both source and destination is 0, which should
        //    cover the vast majority of use cases
        int blit_height_rest = blit_height_total - 1024;
        // first blit
        UINT16 bltsize = (UINT16) (1024 << 6) | blit_width_words;
        bltsize = _blit_rect_simple(dst_addr, src_addr, dstmod, srcmod, bltsize);

        // now the second blit, starting after the stuff, we perform a fast blit for the second
        // blit, since nothing except the blit size and addresses are  changing
        src_addr += src_width_bytes * 1024;
        dst_addr += dst_width_bytes * 1024;
        bltsize = (UINT16) (blit_height_rest << 6) | blit_width_words;
        _blit_rect_simple2(dst_addr, src_addr, bltsize);
        return bltsize;
    } else {
        UINT16 bltsize = (UINT16) ((blit_height_pixels * src->depth) << 6) | blit_width_words;
        return _blit_rect_simple(dst_addr, src_addr, dstmod, srcmod, bltsize);
    }
}

void ratr0_blit_rect_simple2(struct Ratr0Surface *dst,
                             struct Ratr0Surface *src,
                             UINT16 dstx, UINT16 dsty, UINT16 srcx, UINT16 srcy,
                             UINT16 bltsize)
{
    UINT16 src_width_bytes = src->width >> 3;
    UINT16 dst_width_bytes = dst->width >> 3;
    UINT32 src_addr = ((UINT32) src->buffer) + (src_width_bytes * srcy * src->depth) + (srcx >> 3);
    UINT32 dst_addr = ((UINT32) dst->buffer) + (dst_width_bytes * dsty * dst->depth) + (dstx >> 3);
    _blit_rect_simple2(dst_addr, src_addr, bltsize);
}

// Generic D = A blit with shift

UINT16 ratr0_blit_rect(struct Ratr0Surface *dst,
                       struct Ratr0Surface *src,
                       UINT16 dstx, UINT16 dsty,
                       UINT16 srcx, UINT16 srcy,
                       UINT16 blit_width_pixels,
                       UINT16 blit_height_pixels)
{
    UINT16 blit_width_words = blit_width_pixels >> 4;  // blit width in words
    if ((blit_width_pixels & 0x0f) != 0) {
        // It's not evenly dividable by 16 -> add another word
        blit_width_words++;
    }
    UINT16 src_width_bytes = src->width >> 3;
    UINT16 dst_width_bytes = dst->width >> 3;

    // calculate start addresses in both source and destination
    UINT32 src_addr = ((UINT32) src->buffer) + (src_width_bytes * srcy * src->depth) + (srcx >> 3);
    UINT32 dst_addr = ((UINT32) dst->buffer) + (dst_width_bytes * dsty * dst->depth) + (dstx >> 3);
    int blit_height_total = blit_height_pixels * src->depth;

    // Destination variables
    // destination x relative to the containing word, this is also the
    // the shift amount
    INT8 dst_shift = dstx & 0x0f;

    // The destination blit width can be different from the source,
    // depending on where the tile is offset, it can span extra words
    UINT16 start_dst_word = dstx >> 4;
    UINT16 end_dst_word = (dstx + blit_width_pixels) >> 4;
    UINT16 dst_blit_width = end_dst_word - start_dst_word + 1;

    UINT16 alwm = 0xffff;  // ALWM masks the shifted pixels
    UINT16 final_blit_width = blit_width_words;
    if (dst_blit_width > blit_width_words) {
        final_blit_width = dst_blit_width;
        alwm = 0;
    }
    //printf("dst_shift: %d\n", dst_shift);
    // modulos are in *bytes*
    UINT16 srcmod = src_width_bytes - (final_blit_width << 1);
    UINT16 dstmod = dst_width_bytes - (final_blit_width << 1);


    UINT16 bltsize = (UINT16) (blit_height_total << 6) | final_blit_width & 0x3f;
    // The actual blitter part
    WaitBlit();
    // D = A => LF = 0xf0, channels A and D turned on => 0x09
    custom.bltcon0 = 0x09f0 | (dst_shift << 12);
    // not used
    custom.bltcon1 = 0;

    custom.bltafwm = 0xffff;
    custom.bltalwm = alwm;

    custom.bltamod = srcmod;
    custom.bltbmod = 0;
    custom.bltcmod = 0;
    custom.bltdmod = dstmod;

    custom.bltapt = (UINT8 *) src_addr;
    custom.bltbpt = 0;
    custom.bltcpt = 0;
    custom.bltdpt = (UINT8 *) dst_addr;
    custom.bltsize = bltsize;
    return bltsize;
}

// *************************************************************
// **** Cookie cutter blit
// ****************************

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
void _blit_object(UINT32 dst_addr, UINT32 src_addr, UINT32 mask_addr,
                  UINT16 dstmod, UINT16 srcmod,
                  UINT8 num_planes,
                  UINT16 dst_row_bytes, UINT16 src_plane_size,
                  INT8 dst_shift, UINT16 alwm, UINT16 bltsize)
{
    WaitBlit();

    // channels A-D turned on => 0x09 LF => D = AB + ~AC => 0xca
    custom.bltcon0 = 0x0fca | (dst_shift << 12);
    // used
    custom.bltcon1 = dst_shift << 12;

    custom.bltafwm = 0xffff;
    custom.bltalwm = alwm;

    custom.bltamod = srcmod;
    custom.bltbmod = srcmod;
    custom.bltcmod = dstmod;
    custom.bltdmod = dstmod;

    for (int i = 0; i < num_planes; i++) {
        custom.bltapt = (UINT8 *) mask_addr;
        custom.bltbpt = (UINT8 *) src_addr;
        custom.bltcpt = (UINT8 *) dst_addr;
        custom.bltdpt = (UINT8 *) dst_addr;
        custom.bltsize = bltsize;
        WaitBlit();
        src_addr += src_plane_size;
        dst_addr += dst_row_bytes;
    }
}

void ratr0_blit_object(struct Ratr0Surface *dst,
                       struct Ratr0TileSheet *bobs,
                       int tilex, int tiley,
                       int dstx, int dsty)
{
    // Source variables
    UINT16 src_blit_width_pixels = bobs->header.tile_width;
    UINT16 src_blit_width = src_blit_width_pixels / 16;
    if (bobs->header.tile_width & 0x0f != 0) {
        // It's not evenly dividable by 16 -> add another word
        src_blit_width++;
    }

    UINT16 blit_height_pixels = bobs->header.tile_height;
    UINT16 srcx = tilex * bobs->header.tile_width;
    UINT16 srcy = tiley * bobs->header.tile_height;

    // Destination variables
    // destination x relative to the containing word, this is also the
    // the shift amount
    INT8 dst_shift = dstx & 0x0f;

    UINT16 start_dst_word = dstx / 16;
    UINT16 end_dst_word = (dstx + bobs->header.tile_width) / 16;
    UINT16 dst_blit_width = end_dst_word - start_dst_word + 1;

    UINT16 alwm = 0xffff;
    UINT16 final_blit_width = src_blit_width;
    if (dst_blit_width > src_blit_width) {
        final_blit_width = dst_blit_width;
        alwm = 0;
    }
    UINT16 dst_row_bytes = dst->width / 8;

    UINT32 dst_addr = ((UINT32) dst->buffer) + (dst->width / 8 * dsty * dst->depth) + dstx / 8;

    int bobs_plane_size = bobs->header.width / 8 * bobs->header.height;
    UINT8 *bobs_addr = engine->memory_system->block_address(bobs->h_imgdata);
    // non-interleaved -> Offset within the first plane
    UINT32 src_addr = ((UINT32) bobs_addr) + (bobs->header.width / 8 * srcy) + srcx / 8;

    // it's the plane right after the actual image planes
    // base address of the mask + offset of the tile
    UINT32 mask_addr = (UINT32) bobs_addr + bobs_plane_size * bobs->header.bmdepth;
    mask_addr += (bobs->header.width / 8 * srcy) + srcx / 8;

    UINT16 dstmod = dst_row_bytes * dst->depth - (final_blit_width * 2);
    UINT16 srcmod = bobs->header.width / 8 - (final_blit_width * 2);
    UINT8 num_planes = bobs->header.bmdepth;
    UINT16 src_plane_size = bobs->header.width / 8 * bobs->header.height;
    UINT16 bltsize = (UINT16) (blit_height_pixels << 6) | (final_blit_width & 0x3f);
    _blit_object(dst_addr, src_addr, mask_addr, dstmod, srcmod, num_planes,
                 dst_row_bytes, src_plane_size, dst_shift, alwm, bltsize);
}

/**
 * Interleaved version of object blitting
 */
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
void _blit_object_il(UINT32 dst_addr, UINT32 src_addr, UINT32 mask_addr,
                  UINT16 dstmod, UINT16 srcmod,
                  INT8 dst_shift, UINT16 alwm, UINT16 bltsize)
{
    WaitBlit();
    // channels A-D turned on => 0x09 LF => D = AB + ~AC => 0xca
    custom.bltcon0 = 0x0fca | (dst_shift << 12);
    // used
    custom.bltcon1 = dst_shift << 12;

    custom.bltafwm = 0xffff;
    custom.bltalwm = alwm;

    custom.bltamod = srcmod;
    custom.bltbmod = srcmod;
    custom.bltcmod = dstmod;
    custom.bltdmod = dstmod;

    custom.bltapt = (UINT8 *) mask_addr;
    custom.bltbpt = (UINT8 *) src_addr;
    custom.bltcpt = (UINT8 *) dst_addr;
    custom.bltdpt = (UINT8 *) dst_addr;
    custom.bltsize = bltsize;
}


void ratr0_blit_object_il(struct Ratr0Surface *dst,
                                struct Ratr0TileSheet *bobs,
                                int tilex, int tiley,
                                int dstx, int dsty)
{
    int bobs_row_bytes = bobs->header.width >> 3;
    int bobs_plane_size = bobs_row_bytes * bobs->header.height;
    // Source variables
    UINT16 src_blit_width_pixels = bobs->header.tile_width;
    UINT16 src_blit_width_words = src_blit_width_pixels >> 4;
    if ((bobs->header.tile_width & 0x0f) != 0) {
        // It's not evenly dividable by 16 -> add another word
        src_blit_width_words++;
    }

    UINT16 blit_height_pixels = bobs->header.tile_height * bobs->header.bmdepth;
    UINT16 srcx = tilex * bobs->header.tile_width;
    UINT16 srcy = tiley * bobs->header.tile_height;

    // Destination variables
    // destination x relative to the containing word, this is also the
    // the shift amount
    INT8 dst_shift = dstx & 0x0f;

    // The destination blit width can be different from the source,
    // depending on where the tile is offset, it can span extra words
    UINT16 start_dst_word = dstx >> 4;
    UINT16 end_dst_word = (dstx + bobs->header.tile_width) >> 4;
    UINT16 dst_blit_width = end_dst_word - start_dst_word + 1;

    UINT16 alwm = 0xffff;  // ALWM masks the shifted pixels
    UINT16 final_blit_width = src_blit_width_words;
    if (dst_blit_width > src_blit_width_words) {
        final_blit_width = dst_blit_width;
        alwm = 0;
    }
    UINT16 dst_row_bytes = dst->width >> 3;
    UINT32 dst_addr = ((UINT32) dst->buffer) + (dst_row_bytes * dsty * dst->depth) + (dstx >> 3);

    UINT8 *bobs_addr = engine->memory_system->block_address(bobs->h_imgdata);
    // interleaved
    UINT16 tile_offset = (bobs_row_bytes * srcy * bobs->header.bmdepth) + (srcx >> 3);
    UINT32 src_addr = ((UINT32) bobs_addr) + tile_offset;

    // it's the plane right after the actual image planes
    UINT32 mask_addr = (UINT32) bobs_addr + bobs_plane_size * bobs->header.bmdepth;
    mask_addr += tile_offset;

    UINT16 dstmod = dst_row_bytes - (final_blit_width << 1);
    UINT16 srcmod = bobs_row_bytes - (final_blit_width << 1);
    UINT16 bltsize = (UINT16) (blit_height_pixels << 6) | final_blit_width & 0x3f;
    _blit_object_il(dst_addr, src_addr, mask_addr, dstmod, srcmod,
                    dst_shift, alwm, bltsize);
}

UINT16 _blit_8x8(UINT32 dst_addr, UINT32 src_addr, UINT16 dstmod, UINT16 srcmod,
                 UINT8 ashift,
                 UINT16 afwm, UINT16 alwm,
                 UINT16 bltsize)
{
    WaitBlit();
    // D = A + D => LF = 0xfc, channels A, B and D turned on => 0x0d
    custom.bltcon0 = 0x0dfc | (ashift << 12);

    custom.bltafwm = afwm;
    custom.bltalwm = alwm;

    custom.bltamod = srcmod;
    custom.bltbmod = dstmod;
    custom.bltcmod = 0;
    custom.bltdmod = dstmod;

    custom.bltapt = (UINT8 *) src_addr;
    custom.bltbpt = (UINT8 *) dst_addr;
    custom.bltcpt = 0;
    custom.bltdpt = (UINT8 *) dst_addr;
    custom.bltsize = bltsize;
    return bltsize;
}

/*
 * A text blitting function
 */
UINT16 ratr0_blit_8x8(struct Ratr0Surface *dst,
                      struct Ratr0Surface *src,
                      UINT16 dstx, UINT16 dsty,
                      char c,
                      UINT8 plane_num)
{
    // 1, if we don't shift or if the shifted part is in the right half
    // 2, if we shift the left half
    UINT16 blit_width_words = 1;
    UINT16 src_width_bytes = src->width >> 3;
    UINT16 dst_width_bytes = dst->width >> 3;
    UINT16 alwm = 0xffff, afwm = 0xffff;
    int c_index = c - ' ';
    UINT16 srcy = (c_index >> 5) << 3 ;   // div 32 * 8
    UINT16 srcx = (c_index & 0x1f) << 3;  // mod 32 * 8
    // A shift needs to happen if the modulos of dst and src mismatch
    UINT8 ashift = (dstx & 0x0f) != (srcx & 0x0f) ?  8 : 0;

    // Determine which portion of the 16x8 field, left or right
    BOOL use_left = ((srcx & 0x0f) == 0);
    srcx &= 0xfff0; // always align to the word
    if (!use_left && ashift == 8) { // only if we need to shift the right half
        blit_width_words = 2;
    }

    // modulos are in *bytes*
    UINT16 dst_row_bytes = dst->width >> 3;
    UINT16 srcmod = src_width_bytes - (blit_width_words << 1);
    UINT16 dstmod = dst_row_bytes * dst->depth - (blit_width_words << 1);

    UINT32 src_addr = ((UINT32) src->buffer) + (src_width_bytes * srcy * src->depth) + (srcx >> 3);
    UINT32 dst_addr = ((UINT32) dst->buffer) + (dst_width_bytes * dsty * dst->depth) + (dstx >> 3);
    dst_addr += dst_row_bytes * plane_num;  // target plane

    // Shift adjustments
    // For the left half 8 pixel of the 16 pixel field this always works
    if (ashift == 8) {
        if (use_left) { // left half
            alwm = 0xff00;
            afwm = 0xff00;
        } else {
            // Right half: shift needs to be extended by 1, shift by 8 and
            // AFWM/ALWM mask
            dst_addr -= 2;
            afwm = 0x00ff;
            alwm = 0x0000;
        }
    } else { // no shift
        if (use_left) { // left half
            afwm = 0xff00;
            alwm = 0xff00;
        } else {
            afwm = 0x00ff;
            alwm = 0x00ff;
        }
    }
    // We only blit a single plane
    UINT16 bltsize = (UINT16) (8 << 6) | blit_width_words;
    return _blit_8x8(dst_addr, src_addr, dstmod, srcmod,
                     ashift, afwm, alwm,
                     bltsize);
}



void ratr0_blit_rect_1plane(struct Ratr0Surface *dst,
                            struct Ratr0TileSheet *bobs,
                            int tilex, int tiley,
                            int dstx, int dsty)
{
    int bobs_row_bytes = bobs->header.width >> 3;
    int bobs_plane_size = bobs_row_bytes * bobs->header.height;
    // Source variables
    UINT16 src_blit_width_pixels = bobs->header.tile_width;
    UINT16 src_blit_width_words = src_blit_width_pixels >> 4;
    if ((bobs->header.tile_width & 0x0f) != 0) {
        // It's not evenly dividable by 16 -> add another word
        src_blit_width_words++;
    }

    UINT16 blit_height_pixels = bobs->header.tile_height;  // only 1 plane !!!
    UINT16 srcx = tilex * bobs->header.tile_width;
    UINT16 srcy = tiley * bobs->header.tile_height;

    // Destination variables
    // destination x relative to the containing word, this is also the
    // the shift amount
    INT8 dst_shift = 0; // TODO: add me - dstx & 0x0f;

    // The destination blit width can be different from the source,
    // depending on where the tile is offset, it can span extra words
    UINT16 start_dst_word = dstx >> 4;
    UINT16 end_dst_word = (dstx + bobs->header.tile_width) >> 4;
    UINT16 dst_blit_width = end_dst_word - start_dst_word + 1;

    UINT16 alwm = 0xffff;  // ALWM masks the shifted pixels
    UINT16 final_blit_width = src_blit_width_words;
    if (dst_blit_width > src_blit_width_words) {
        final_blit_width = dst_blit_width;
        alwm = 0;
    }
    UINT16 dst_row_bytes = dst->width >> 3;
    UINT32 dst_addr = ((UINT32) dst->buffer) + (dst_row_bytes * dsty * dst->depth) + (dstx >> 3);

    UINT8 *bobs_addr = engine->memory_system->block_address(bobs->h_imgdata);
    // interleaved
    UINT16 tile_offset = (bobs_row_bytes * srcy * bobs->header.bmdepth) + (srcx >> 3);
    UINT32 src_addr = ((UINT32) bobs_addr) + tile_offset;

    // it's the plane right after the actual image planes
    UINT32 mask_addr = (UINT32) bobs_addr + bobs_plane_size * bobs->header.bmdepth;
    mask_addr += tile_offset;

    UINT16 dstmod = dst_row_bytes - (final_blit_width << 1);
    UINT16 srcmod = bobs_row_bytes - (final_blit_width << 1);
    UINT16 bltsize = (UINT16) (blit_height_pixels << 6) | final_blit_width & 0x3f;

    WaitBlit();
    // D = A => LF = 0xf0, channels A and D turned on => 0x09
    custom.bltcon0 = 0x09f0 | (dst_shift << 12);
    // used
    custom.bltcon1 = dst_shift << 12;

    custom.bltafwm = 0xffff;
    custom.bltalwm = alwm;

    custom.bltamod = srcmod;
    custom.bltdmod = dstmod;

    custom.bltapt = (UINT8 *) src_addr;
    custom.bltdpt = (UINT8 *) dst_addr;
    custom.bltsize = bltsize;
}
