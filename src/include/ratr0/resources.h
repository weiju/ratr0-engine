/** @file resources.h
 *
 * Resources subsystem
 */
#pragma once
#ifndef __RATR0_RESOURCES_H__
#define __RATR0_RESOURCES_H__
#include <ratr0/data_types.h>
#include <ratr0/engine.h>
#include <ratr0/memory.h>

/** \brief length of file identifier */
#define FILE_ID_LEN (8)

/** \brief file is encoded in little endian format */
#define TSFLAGS_LITTLE_ENDIAN     (1)
/** \brief image is encoded in RGB instead of palette */
#define TSFLAGS_RGB               (2)
/** \brief image data is non-interleaved */
#define TSFLAGS_NON_INTERLEAVED   (4)
/** \brief image data is followed by blitter mask */
#define TSFLAGS_HAS_MASK          (8)

/**
 * information about a tile sheet
 * File format version 2
 * changes to version 1:
 *   1. dropped the reserved2 word after palette_size
 *   2. changed size of checksum from UINT32 to UINT16
 */
struct Ratr0TileSheetHeader {
    /** \brief file identifier */
    UINT8 id[FILE_ID_LEN];
    /** \brief file format version */
    UINT8 version;
    /** \brief file flags */
    UINT8 flags;
    /** \brief reserved byte, don't use */
    UINT8 reserved1;
    /** \brief number of bitplanes */
    UINT8 bmdepth;
    /** \brief image width */
    UINT16 width;
    /** \brief image height */
    UINT16 height;
    /** \brief tile width */
    UINT16 tile_width;
    /** \brief tile height */
    UINT16 tile_height;
    /** \brief number of horizontal tiles in the sheet */
    UINT16 num_tiles_h;
    /** \brief number of vertical tiles in the sheet */
    UINT16 num_tiles_v;
    /** \brief number of palette entries */
    UINT16 palette_size;
    /** \brief size of image data in bytes */
    UINT32 imgdata_size;
    /** \brief checksum */
    UINT16 checksum;
};

/** \brief maximum palette size */
#define MAX_PALETTE_SIZE (32)

/**
 * Ratr0TileSheet data structure.
 */
struct Ratr0TileSheet {
    /** \brief tile sheet information header */
    struct Ratr0TileSheetHeader header;
    /** \brief image palette */
    UINT16 palette[MAX_PALETTE_SIZE];
    /** \brief handle to image data */
    Ratr0MemHandle h_imgdata;
};


struct Ratr0SpriteSheetHeader {
    /** \brief file identifier */
    UINT8 id[FILE_ID_LEN];
    /** \brief file format version */
    UINT8 version;
    /** \brief file flags */
    UINT8 flags;
    /** \brief reserved byte, don't use */
    UINT8 reserved1;
    /** \brief number of palette entries */
    UINT8 num_colors;
    /** \brief number of sprites */
    UINT16 num_sprites;
    /** \brief size of image data in bytes */
    UINT32 imgdata_size;
    /** \brief checksum */
    UINT16 checksum;
};

struct Ratr0SpriteSheet {
    /** \brief tile sheet information header */
    struct Ratr0SpriteSheetHeader header;
    /** \brief num_sprites 16-bit words of offsets and palette_size 16 bit words of
     *   color data
     */
    UINT16 *sprite_offsets;
    UINT16 *colors;

    /** \brief handle to image data */
    Ratr0MemHandle h_imgdata;
};

struct Ratr0AudioSample {
    /** \brief number of bytes in the sample */
    UINT32 num_bytes; // round up to even number of  bytes

    /** \brief handle to audio sample data */
    Ratr0MemHandle h_data;
};

struct Ratr0AudioProtrackerMod {
    /** \brief handle to Protracker mod data */
    Ratr0MemHandle h_data;
};

/**
 * Reads a tilesheet from the file system.
 *
 * @param filename the path to the tilesheet file
 * @param sheet pointer to an unitialized tilesheet structure
 * @return FALSE if error, TRUE if success
 */
extern BOOL ratr0_resources_read_tilesheet(const char *filename,
                                           struct Ratr0TileSheet *sheet);
/**
 * Frees the data in a tilesheet and returns it to the memory system.
 *
 * @param sheet pointer to an initialized tilesheet
 */
extern void ratr0_resources_free_tilesheet_data(struct Ratr0TileSheet *sheet);

/**
 * Initializes a surface data structure from a tile sheet.
 *
 * @param surface pointer to an uninitialized surface
 * @param sheet pointer to a tile sheet
 */
extern void ratr0_resources_init_surface_from_tilesheet(struct Ratr0Surface *surface,
                                                        struct Ratr0TileSheet *sheet);

/**
 * Reads a sprite sheet from the file system.
 *
 * @param filename the path to the sprite sheet file
 * @param sheet pointer to an unitialized sprite sheet structure
 * @return FALSE if error, TRUE if success
 */
extern BOOL ratr0_resources_read_spritesheet(const char *filename,
                                             struct Ratr0SpriteSheet *sheet);

/**
 * Frees the data in a sprite sheet and returns it to the memory system.
 *
 * @param sheet pointer to an initialized sprite sheet
 */
extern void ratr0_resources_free_spritesheet_data(struct Ratr0SpriteSheet *sheet);

/**
 * Reads a raw audio sample from the file system.
 *
 * @param filename the path to the sound sample file
 * @param sample pointer to an uninitialized sample structure
 * @return FALSE if error, TRUE if success
 */
extern BOOL ratr0_resources_read_audiosample(const char *filename,
                                             struct Ratr0AudioSample *sample);

/**
 * Frees the data in an audio sample and returns it to the memory system.
 *
 * @param sample pointer to an initialized audio sample
 */
extern void ratr0_resources_free_audiosample_data(struct Ratr0AudioSample *sample);

/**
 * Reads a protracker module from the file system.
 *
 * @param filename the path to the Protracker module
 * @param sample pointer to an uninitialized mod structure
 * @return FALSE if error, TRUE if success
 */
extern BOOL ratr0_resources_read_protracker(const char *filename,
                                            struct Ratr0AudioProtrackerMod *mod);

/**
 * Frees the data in a protracker module  and returns it to the memory system.
 *
 * @param mod pointer to an initialized Protracker module
 */
extern void ratr0_resources_free_protracker_data(struct Ratr0AudioProtrackerMod *mod);

/**
 * Interface to resource subsystem.
 */
struct Ratr0ResourceSystem {
    /**
     * Shuts down the resources subsystem.
     */
    void (*shutdown)(void);
};

/**
 * Start up the resources subsystem.
 *
 * @param engine pointer to engine instance
 * @return pointer to initialized Ratr0ResourceSystem
 */
extern struct Ratr0ResourceSystem *ratr0_resources_startup(Ratr0Engine *engine);

#endif /* __RATR0_RESOURCES_H__ */
