#include <hardware/custom.h>
#include <clib/graphics_protos.h>

#include <ratr0/debug_utils.h>
#include <ratr0/resources.h>
#include <ratr0/amiga/blitter.h>
#include <ratr0/amiga/display.h>

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

void ratr0_amiga_blitter_startup(Ratr0Engine *eng)
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
UINT16 _blit_rect(UINT32 dst_addr, UINT32 src_addr, UINT16 dstmod, UINT16 srcmod,
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
void _blit_rect_fast(UINT32 dst_addr, UINT32 src_addr, UINT16 bltsize)
{
    WaitBlit();
    custom.bltapt = (UINT8 *) src_addr;
    custom.bltdpt = (UINT8 *) dst_addr;
    custom.bltsize = bltsize;
}

void ratr0_amiga_do_blit_command(struct Ratr0AmigaBlitCommand *cmd)
{
    if (cmd->blit_type == BLIT_BLOCK) {
        _blit_rect(cmd->dst_addr, cmd->src_addr, cmd->dstmod, cmd->srcmod,
                   cmd->bltsize);
    } else if (cmd->blit_type == BLIT_BOB) {
        _blit_object(cmd->dst_addr, cmd->src_addr, cmd->mask_addr,
                     cmd->dstmod, cmd->srcmod, cmd->num_planes,
                     cmd->dst_row_bytes, cmd->src_plane_size,
                     cmd->dst_shift, cmd->alwm, cmd->bltsize);
    } else if (cmd->blit_type == BLIT_BOB_IL) {
        _blit_object_il(cmd->dst_addr, cmd->src_addr, cmd->mask_addr, cmd->dstmod, cmd->srcmod,
                        cmd->dst_shift, cmd->alwm, cmd->bltsize);
    }
}

void ratr0_amiga_make_blit_rect(struct Ratr0AmigaBlitCommand *cmd,
                                struct Ratr0AmigaSurface *dst,
                                struct Ratr0AmigaSurface *src,
                                UINT16 dstx, UINT16 dsty, UINT16 srcx, UINT16 srcy,
                                UINT16 blit_width_pixels, UINT16 blit_height_pixels)
{
    cmd->blit_type = BLIT_BLOCK;

    // TODO
}

UINT16 ratr0_amiga_blit_rect(struct Ratr0AmigaSurface *dst,
                           struct Ratr0AmigaSurface *src,
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
    UINT16 bltsize = (UINT16) ((blit_height_pixels * src->depth) << 6) | blit_width_words;

    return _blit_rect(dst_addr, src_addr, dstmod, srcmod, bltsize);
}

void ratr0_amiga_blit_rect_fast(struct Ratr0AmigaSurface *dst,
                                struct Ratr0AmigaSurface *src,
                                UINT16 dstx, UINT16 dsty, UINT16 srcx, UINT16 srcy,
                                UINT16 bltsize)
{
    UINT16 src_width_bytes = src->width >> 3;
    UINT16 dst_width_bytes = dst->width >> 3;
    UINT32 src_addr = ((UINT32) src->buffer) + (src_width_bytes * srcy * src->depth) + (srcx >> 3);
    UINT32 dst_addr = ((UINT32) dst->buffer) + (dst_width_bytes * dsty * dst->depth) + (dstx >> 3);
    _blit_rect_fast(dst_addr, src_addr, bltsize);
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

void ratr0_amiga_make_blit_object(struct Ratr0AmigaBlitCommand *cmd,
                                  struct Ratr0AmigaSurface *dst,
                                  struct Ratr0TileSheet *bobs,
                                  int tilex, int tiley,
                                  int dstx, int dsty)
{
}


void ratr0_amiga_blit_object(struct Ratr0AmigaSurface *dst,
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


void ratr0_amiga_blit_object_il(struct Ratr0AmigaSurface *dst,
                                struct Ratr0TileSheet *bobs,
                                int tilex, int tiley,
                                int dstx, int dsty)
{
    int bobs_row_bytes = bobs->header.width >> 3;
    int bobs_plane_size = bobs_row_bytes * bobs->header.height;
    // Source variables
    UINT16 src_blit_width_pixels = bobs->header.tile_width;
    UINT16 src_blit_width_words = src_blit_width_pixels >> 4;
    if (bobs->header.tile_width & 0x0f != 0) {
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


void ratr0_amiga_make_blit_object_il(struct Ratr0AmigaBlitCommand *cmd,
                                    struct Ratr0AmigaSurface *dst,
                                    struct Ratr0TileSheet *bobs,
                                    int tilex, int tiley,
                                    int dstx, int dsty)
{
}
