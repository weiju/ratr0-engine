/** @file resources.c */
#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/memory.h>
#include <ratr0/resources.h>

#ifdef AMIGA
#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[33;1mRESOURCES\033[0m", __VA_ARGS__)
#else
#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[34;1mRESOURCES\033[0m", __VA_ARGS__)
#endif

/**
 * Endianess swapping on Intel machines.
 */
static UINT16 byteswap16(UINT16 value)
{
    return ((value & 0x00ff) << 8) | ((value & 0xff00) >> 8);
}

static UINT32 byteswap32(UINT32 value)
{
    return ((value & 0xff00ff00) >> 8) | ((value & 0x00ff00ff) << 8);
}

void ratr0_resources_shutdown(void);

static struct Ratr0ResourceSystem resource_system;
static Ratr0Engine *engine;
#define MAX_INFO_WORDS (40)
UINT16 info_words[MAX_INFO_WORDS];
UINT16 num_info_words;

struct Ratr0ResourceSystem *ratr0_resources_startup(Ratr0Engine *eng)
{
    engine = eng;
    resource_system.shutdown = &ratr0_resources_shutdown;
    num_info_words = 0;

    PRINT_DEBUG("Startup finished.");
    return &resource_system;
}

void ratr0_resources_shutdown(void)
{
    PRINT_DEBUG("Shutdown finished.");
}

BOOL ratr0_resources_read_tilesheet(const char *filename,
                                    struct Ratr0TileSheet *sheet)
{
    int elems_read;
    FILE *fp = fopen(filename, "rb");

    if (fp) {
        int num_img_bytes;
        elems_read = fread(&sheet->header, sizeof(struct Ratr0TileSheetHeader), 1, fp);
        // Palette size is in big endian, twizzle to little endian on Intel
#ifdef AMIGA
        UINT16 palette_size = sheet->header.palette_size;
        UINT32 imgdata_size = sheet->header.imgdata_size;
#else
        UINT16 palette_size = byteswap16(sheet->header.palette_size);
        UINT32 imgdata_size = byteswap32(sheet->header.imgdata_size);
        printf("palette size: %d imgdata_size: %d\n", palette_size, imgdata_size);
#endif
        elems_read = fread(&sheet->palette, sizeof(UINT16), palette_size, fp);
        Ratr0MemHandle handle = ratr0_memory_allocate_block(RATR0_MEM_CHIP, imgdata_size);
        sheet->h_imgdata = handle;
        UINT8 *imgdata = ratr0_memory_block_address(handle);
        elems_read = fread(imgdata, sizeof(unsigned char), imgdata_size, fp);
        fclose(fp);
        return TRUE;
    } else {
        printf("ratr0_read_tilesheet() error: file '%s' not found\n", filename);
        return FALSE;
    }
}

/**
 * Frees the memory that was allocated for the specified RATR0 tile sheet.
 */
void ratr0_resources_free_tilesheet_data(struct Ratr0TileSheet *sheet)
{
    if (sheet && sheet->h_imgdata) ratr0_memory_free_block(sheet->h_imgdata);
}

BOOL ratr0_resources_read_spritesheet(const char *filename, struct Ratr0SpriteSheet *sheet)
{
    int elems_read;
    FILE *fp = fopen(filename, "rb");

    if (fp) {
        int num_img_bytes;
        elems_read = fread(&sheet->header, sizeof(struct Ratr0SpriteSheetHeader), 1, fp);
        // Palette size is in big endian, twizzle to little endian on Intel
        UINT8 palette_size = sheet->header.num_colors;
#ifdef AMIGA
        UINT32 imgdata_size = sheet->header.imgdata_size;
#else
        UINT32 imgdata_size = byteswap32(sheet->header.imgdata_size);
#endif
        printf("palette size: %d imgdata_size: %d\n", (int) palette_size, (int) imgdata_size);
        // TODO: 0. reserve info chunk memory for offsets and colors
        sheet->sprite_offsets = &info_words[num_info_words];
        num_info_words += sheet->header.num_sprites;
        sheet->colors = &info_words[num_info_words];
        num_info_words += palette_size;

        // 1. Read offsets
        elems_read = fread(sheet->sprite_offsets, sizeof(UINT16),
                           sheet->header.num_sprites, fp);
        // 2. read sprite colors
        elems_read = fread(sheet->colors, sizeof(UINT16), palette_size, fp);

        // 3. read image data
        Ratr0MemHandle handle = ratr0_memory_allocate_block(RATR0_MEM_CHIP,
                                                            imgdata_size);
        sheet->h_imgdata = handle;
        UINT8 *imgdata = ratr0_memory_block_address(handle);
        elems_read = fread(imgdata, sizeof(unsigned char), imgdata_size, fp);
        fclose(fp);
        return TRUE;
    } else {
        printf("ratr0_read_spritesheet() error: file '%s' not found\n", filename);
        return FALSE;
    }
}

/**
 * Frees the memory that was allocated for the specified RATR0 sprite sheet.
 */
void ratr0_resources_free_spritesheet_data(struct Ratr0SpriteSheet *sheet)
{
    if (sheet && sheet->h_imgdata) ratr0_memory_free_block(sheet->h_imgdata);
}

BOOL ratr0_resources_read_audiosample(const char *filename,
                                      struct Ratr0AudioSample *sample)
{
    FILE *fp = fopen(filename, "rb");
    if (fp) {
        UINT32 file_offset = fseek(fp, 0, SEEK_END);
        UINT32 filesize = ftell(fp);
        if ((filesize % 2) == 1) {
            filesize++;
        }
        sample->h_data = ratr0_memory_allocate_block(RATR0_MEM_CHIP,
                                                     filesize);
        sample->num_bytes = filesize;

        // read sample data into memory
        UINT8 *sampledata = ratr0_memory_block_address(sample->h_data);
        file_offset = fseek(fp, 0, SEEK_SET);
        UINT32 elems_read = fread(sampledata, sizeof(UINT8), filesize, fp);
        fclose(fp);
        // Make sure the first 2 bytes are 0 for PTPlayer to properly work
        if (filesize >  2) {
            sampledata[0] = sampledata[1] = 0;
        }
        return TRUE;
    } else {
        return FALSE;
    }
}

void ratr0_resources_free_audiosample_data(struct Ratr0AudioSample *sample)
{
    if (sample && sample->h_data) ratr0_memory_free_block(sample->h_data);
}

BOOL ratr0_resources_read_protracker(const char *filename,
                                     struct Ratr0AudioProtrackerMod *mod)
{
    FILE *fp = fopen(filename, "rb");
    if (fp) {
        UINT32 file_offset = fseek(fp, 0, SEEK_END);
        UINT32 filesize = ftell(fp);
        mod->h_data = ratr0_memory_allocate_block(RATR0_MEM_CHIP,
                                                     filesize);

        // read sample data into memory
        UINT8 *moddata = ratr0_memory_block_address(mod->h_data);
        file_offset = fseek(fp, 0, SEEK_SET);
        UINT32 elems_read = fread(moddata, sizeof(UINT8), filesize, fp);
        fclose(fp);
        return TRUE;
    } else {
        return FALSE;
    }
}

void ratr0_resources_free_protracker_data(struct Ratr0AudioProtrackerMod *mod)
{
    if (mod && mod->h_data) ratr0_memory_free_block(mod->h_data);
}
