#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/memory.h>
#include <ratr0/resources.h>

#ifdef AMIGA
#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[33;1mRESOURCES\033[0m", __VA_ARGS__)
#else
#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[34;1mRESOURCES\033[0m", __VA_ARGS__)
#endif

void ratr0_resources_shutdown(void);
UINT32 ratr0_read_tilesheet(const char *filename, struct Ratr0TileSheet *sheet);
void ratr0_free_tilesheet_data(struct Ratr0TileSheet *sheet);

static struct Ratr0ResourceSystem resource_system;
static Ratr0Engine *engine;

struct Ratr0ResourceSystem *ratr0_resources_startup(Ratr0Engine *eng)
{
    engine = eng;
    resource_system.shutdown = &ratr0_resources_shutdown;
    resource_system.read_tilesheet = &ratr0_read_tilesheet;
    resource_system.free_tilesheet_data = &ratr0_free_tilesheet_data;
    PRINT_DEBUG("Startup finished.");
    return &resource_system;
}

void ratr0_resources_shutdown(void)
{
    PRINT_DEBUG("Shutdown finished.");
}

UINT32 ratr0_read_tilesheet(const char *filename, struct Ratr0TileSheet *sheet)
{
    int elems_read;
    UINT32 retval = 0;

    FILE *fp = fopen(filename, "rb");

    if (fp) {
        int num_img_bytes;
        elems_read = fread(&sheet->header, sizeof(struct Ratr0TileSheetHeader), 1, fp);
        elems_read = fread(&sheet->palette, sizeof(UWORD), sheet->header.palette_size, fp);
        Ratr0MemHandle handle = engine->memory_system->allocate_block(RATR0_MEM_CHIP, sheet->header.imgdata_size);
        sheet->h_imgdata = handle;
        UINT8 *imgdata = engine->memory_system->block_address(handle);
        elems_read = fread(imgdata, sizeof(unsigned char), sheet->header.imgdata_size, fp);
        fclose(fp);
        return 1;
    } else {
        printf("ratr0_read_tilesheet() error: file '%s' not found\n", filename);
        return 0;
    }
}

/**
 * Frees the memory that was allocated for the specified RATR0 tile sheet.
 */
void ratr0_free_tilesheet_data(struct Ratr0TileSheet *sheet)
{
    if (sheet && sheet->h_imgdata) engine->memory_system->free_block(sheet->h_imgdata);
}
