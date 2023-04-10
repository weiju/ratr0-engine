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
static Ratr0MemHandle h_sprite_data, h_sprite_table;
static UINT16 *sprite_data;
static UINT16 **sprite_data_table;

static UINT16 first_free_mem; // First available memory block offset (a *byte* offset !!!)
static UINT16 first_free_idx;
static UINT16 pool_size, max_sprites;
static Ratr0Engine *engine;

typedef UINT16 Ratr0SpriteHandle;

void  ratr0_amiga_sprites_startup(Ratr0Engine *eng, UINT16 _pool_size, UINT16 _max_sprites)
{
    engine = eng;
    pool_size = _pool_size;
    max_sprites = _max_sprites;

    h_sprite_data = engine->memory_system->allocate_block(RATR0_MEM_CHIP, pool_size);
    h_sprite_table = engine->memory_system->allocate_block(RATR0_MEM_DEFAULT, max_sprites * sizeof(UINT16 *));
    sprite_data = engine->memory_system->block_address(h_sprite_data);
    sprite_data_table = engine->memory_system->block_address(h_sprite_table);
    first_free_mem = 0;
    first_free_idx = 0;
}

void  ratr0_amiga_sprites_shutdown(void)
{
    engine->memory_system->free_block(h_sprite_data);
    engine->memory_system->free_block(h_sprite_table);
}

Ratr0SpriteHandle allocate_sprite_data(UINT16 num_words)
{
    // TODO: This is not complete, we can't support deallocation yet !!!
    Ratr0SpriteHandle result = first_free_idx;
    UINT16 *memaddr = (UINT16 *) ((UINT32) sprite_data + first_free_mem);
    first_free_mem += num_words / 2;
    sprite_data_table[first_free_idx++] = memaddr;
    return result;
}


/**
 * This function extracts the specified frames from a Ratr0TileSheet and arranges it
 * into a sprite data structure.
 */
UINT16 *ratr0_amiga_make_sprite_data(struct Ratr0TileSheet *tilesheet, UINT8 *frames, UINT8 num_frames)
{
    // for now I assume the sprite data is arranged in 16 pixel width and being 2 bits deep
    // we only support 2 or 4 bits
    int imgdata_words = (tilesheet->header.width / 8) * (tilesheet->header.bmdepth / 2) *
        tilesheet->header.height;
    int words_to_reserve = imgdata_words + 4;
    PRINT_DEBUG("WORDS TO RESERVE: %d", words_to_reserve);
    UINT16 *imgdata = (UINT16 *) engine->memory_system->block_address(tilesheet->h_imgdata);

    Ratr0SpriteHandle sprite_handle = allocate_sprite_data(words_to_reserve);
    UINT16 *sprite_data = sprite_data_table[sprite_handle];
    PRINT_DEBUG("SPRITE DATA AT: %08x", (int) sprite_data);

    /*
static void set_sprite_pos(UWORD *sprite_data, UWORD hstart, UWORD vstart, UWORD vstop)
{
    sprite_data[0] = ((vstart & 0xff) << 8) | ((hstart >> 1) & 0xff);
    // vstop + high bit of vstart + low bit of hstart
    sprite_data[1] = ((vstop & 0xff) << 8) |  // vstop 8 low bits
        ((vstart >> 8) & 1) << 2 |  // vstart high bit
        ((vstop >> 8) & 1) << 1 |   // vstop high bit
        (hstart & 1) |              // hstart low bit
        sprite_data[1] & 0x80;      // preserve attach bit
}
    */
    // 1. initialize the sprite control words
    UINT16 hstart = 300;
    UINT16 vstart = 200;
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
