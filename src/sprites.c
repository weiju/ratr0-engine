/** @file sprites.c */
#include <ratr0/debug_utils.h>
#include <ratr0/memory.h>
#include <ratr0/resources.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[32mSPRITES\033[0m", __VA_ARGS__)
/*
 * Amiga sprite module.
 * Amiga sprites are a data structure that starts with 2 descriptor words,
 * and data for a single bitplane follows immediately,
 * and ending with a pair of closing words
 * Simple sprites are 2 bits deep and each line is a pair of
 * data words, 1. the high order word and 2. the low order word
 * Attached spritess are 4 bits deep and the data is interleaved.
 */

// The chunk of engine memory that we allocate for our sprites
// We also maintain a table of sprite data structure entries
static Ratr0Engine *engine;

void  ratr0_sprites_startup(Ratr0Engine *eng)
{
    engine = eng;
}

void  ratr0_sprites_shutdown(void)
{
}

void ratr0_sprites_set_pos(UINT16 *sprite_data, UINT16 hstart, UINT16 vstart, UINT16 vstop)
{
    sprite_data[0] = ((vstart & 0xff) << 8) | ((hstart >> 1) & 0xff);
    // vstop + high bit of vstart + low bit of hstart
    sprite_data[1] = ((vstop & 0xff) << 8) |  // vstop 8 low bits
        ((vstart >> 8) & 1) << 2 |  // vstart high bit
        ((vstop >> 8) & 1) << 1 |   // vstop high bit
        (hstart & 1) |              // hstart low bit
        sprite_data[1] & 0x80;      // preserve attach bit
}

/**
 * This function extracts the specified frames from a Ratr0TileSheet and arranges it
 * into a sprite data structure.
 */
UINT16 *ratr0_make_sprite_data(struct Ratr0TileSheet *tilesheet, UINT8 frames[],
                                     UINT8 num_frames)
{
    // for now I assume the sprite data is arranged in 16 pixel width and being 2 bits deep
    // we only support 2 or 4 bits
    int imgdata_words = (tilesheet->header.width / 8) * (tilesheet->header.bmdepth / 2) *
        tilesheet->header.height;
    int words_to_reserve = imgdata_words + 4;
    PRINT_DEBUG("WORDS TO RESERVE: %d", words_to_reserve);
    UINT16 *imgdata = (UINT16 *) engine->memory_system->block_address(tilesheet->h_imgdata);

    Ratr0MemHandle sprite_handle = engine->memory_system->allocate_block(RATR0_MEM_CHIP,
                                                                         words_to_reserve * sizeof(UINT16));
    UINT16 *sprite_data = engine->memory_system->block_address(sprite_handle);

    /*
    */
    // 1. initialize the sprite control words
    // These are the 0 positions of sprites given our display settings
    UINT16 hstart = 128;
    UINT16 vstart = 44;
    UINT16 vstop = vstart + 16;  // TODO: height can be anything

    sprite_data[0] = ((vstart & 0xff) << 8) | ((hstart >> 1) & 0xff);
    // vstop + high bit of vstart + low bit of hstart
    sprite_data[1] = ((vstop & 0xff) << 8) |  // vstop 8 low bits
        ((vstart >> 8) & 1) << 2 |  // vstart high bit
        ((vstop >> 8) & 1) << 1 |   // vstop high bit
        (hstart & 1);              // hstart low bit

    // 2. copy tilesheet data to sprite image data
    for (int i = 0; i < imgdata_words; i++) {
        sprite_data[2 + i] = imgdata[i];
    }
    // 3. finish the data structure
    sprite_data[words_to_reserve - 2] = 0;
    sprite_data[words_to_reserve - 1] = 0;
    return sprite_data;
}
