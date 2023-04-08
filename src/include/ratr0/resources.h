#pragma once
#ifndef __RATR0_RESOURCES_H__
#define __RATR0_RESOURCES_H__
#include <ratr0/data_types.h>
#include <ratr0/engine.h>
#include <ratr0/memory.h>

/* Resources subsystem */
// information about a tile sheet
#define FILE_ID_LEN (8)

// information about a tile sheet
// File format version 2
// changes to version 1:
//   1. dropped the reserved2 word after palette_size
//   2. changed size of checksum from ULONG to UWORD
struct Ratr0TileSheetHeader {
    UINT8 id[FILE_ID_LEN];
    UINT8 version, flags;
    UINT8 reserved1, bmdepth;
    UINT16 width, height;
    UINT16 tile_width, tile_height;
    UINT16 num_tiles_h, num_tiles_v;
    UINT16 palette_size;
    UINT32 imgdata_size;
    UINT16 checksum;
};

#define MAX_PALETTE_SIZE (32)
struct Ratr0TileSheet {
    struct Ratr0TileSheetHeader header;
    UINT16 palette[MAX_PALETTE_SIZE];
    /* Use memory handle */
    Ratr0MemHandle h_imgdata;
};

struct Ratr0ResourceSystem {
    UINT32 (*read_tilesheet)(const char *filename, struct Ratr0TileSheet *sheet);
    void (*free_tilesheet_data)(struct Ratr0TileSheet *sheet);
    void (*shutdown)(void);
};

/**
 * Start up the resources subsystem.
 */
extern struct Ratr0ResourceSystem *ratr0_resources_startup(Ratr0Engine *);

#endif /* __RATR0_RESOURCES_H__ */
