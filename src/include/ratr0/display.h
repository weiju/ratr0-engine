/** @file display.h
 *
 * Amiga Display subsystem. Captures the basic aspects of the display
 * system and serves as the top level display API for the RATR0 engine:
 *
 *   - Playfields
 *   - Copper lists
 *   - Sprites
 *   - Blitter related things (BOBs, tiles, ...)
 */
#pragma once
#ifndef __RATR0_DISPLAY_H__
#define __RATR0_DISPLAY_H__
#include <ratr0/data_types.h>
#include <ratr0/engine.h>


// DDFSTRT (lores) = DIWSTRT_h / 2 - 8.5
// so $81 -> $38
// DDFSTRT = DDFSTOP-(8*(word count-1))for low resolution
// DDFSTOP = DDFSTRT + (8 * (word count - 1))
// Data fetch for a 320 pixel wide image: DDFSTRT = 0x38, DDFSTOP = 0xd0
// Data fetch for a 288 pixel wide image: DDFSTRT = 0x46, DDFSTOP = 0xce
#define DDFSTRT_VALUE_320      0x0038
#define DDFSTOP_VALUE_320      0x00d0

// 1. 40/c8
#define DDFSTRT_VALUE_288      0x0040
#define DDFSTOP_VALUE_288      0x00c8

// Display window
#define DIWSTRT_VALUE_320      0x2c81
#define DIWSTOP_VALUE_PAL_320  0x2cc1
#define DIWSTOP_VALUE_NTSC_320 0xf4c1

// DDFSTRT = DIWSTRT / 2 - 8.5
// DIWSTRT = (DDFSTRT + 8.5) * 2
// DIWSTRT_h = (DDFSTRT_h + 8.5) * 2
// 288 horizontal centered
#define DIWSTRT_VALUE_288      0x2c91
#define DIWSTOP_VALUE_PAL_288  0x2cb1
#define DIWSTOP_VALUE_NTSC_288 0xf4b1

// These values make the sprite positions consistent with BOB positions,
// provided we have a 320 pixel wide display
#define DISP_SPRITE_X0_320 (128)
#define DISP_SPRITE_Y0 (44)

#define MAX_PLAYFIELDS (2)
#define MAX_BITPLANES (6)

#ifdef AMIGA
extern UINT16 __chip NULL_SPRITE_DATA[];
#endif


struct Ratr0PlayfieldInfo {

    /** Playfield buffer widths */
    /** \brief display buffer width */
    UINT16 buffer_width;
    /** \brief display buffer height */
    UINT16 buffer_height;

    /** \brief display depth, can be a value between 1 and 5 */
    UINT8 depth;

    /** \brief number of display buffer, this is the main buffer + any
     amount of back buffers for the playfield */
    UINT8 num_buffers;

    /**
     * \brief number of frames to switch buffers
     *
     * How many frames to update the backbuffer ? For now, this
     * should only be either 1 or 2. More than that heavily impacts
     * gameplay experience. Currently it is always 1
     */
    UINT8 update_frames;
};

struct Ratr0DisplayInit {
    /**
     * \brief viewport width
     *
     * Width is a multiple of 16 and should be <= 320. Height can be max 200 for NTSC and
     * 256 for PAL. Smaller values will typically result in less memory consumption and faster
     * refresh times.
     * Sensible values for width can be { 320, 288 }
     * Sensible values for height can be { 192, 208, 224, 240 }
     */
    UINT16 vp_width;
    /** \brief viewport height */
    UINT16 vp_height;

    /** number of playfields used */
    UINT16 num_playfields;

    /** \brief playfield descriptions  */
    struct Ratr0PlayfieldInfo playfield[MAX_PLAYFIELDS];
};

/**
 * Amiga specific information about the display, used both for
 * initialization and query information. This includes aspects of the
 * playfield hardware, sprites and the blitter.
 */
struct Ratr0DisplayInfo {
    /**
     * \brief viewport width
     *
     * Width is a multiple of 16 and should be <= 320. Height can be max 200 for NTSC and
     * 256 for PAL. Smaller values will typically result in less memory consumption and faster
     * refresh times.
     * Sensible values for width can be { 320, 288 }
     * Sensible values for height can be { 192, 208, 224, 240 }
     */
    UINT16 vp_width;
    /** \brief viewport height */
    UINT16 vp_height;

    /** number of playfields used */
    UINT16 num_playfields;

    /** \brief playfield descriptions  */
    struct Ratr0PlayfieldInfo playfield[MAX_PLAYFIELDS];

    /** \brief if PAL, this is TRUE, if NTSC, this is FALSE */
    BOOL is_pal;
};

extern struct Ratr0DisplayInfo display_info;

/**
 * Surface is a rendering target, it is an abstract thing.
 */
struct Ratr0Surface;

/**
 * Interface to the rendering subsystem.
 */
struct Ratr0RenderingSystem {
    /**
     * Shuts down the rendering subsystem.
     */
    void (*shutdown)(void);
};

/**
 * Start up the display subsystem.
 *
 * @param engine pointer to engine instance
 * @param display_info configuration informatino for the display.
 * @return pointer to rendering subsystem object
 */
struct Ratr0RenderingSystem *ratr0_display_startup(Ratr0Engine *eng);

/**
 * Information about the current display in this object.
 * We can use this for both playfield hardware setup and blitter setup.
 * In the future we can use this for backbuffer and/or dual playfield
 * information.
 */
struct Ratr0Surface {
    /** \brief surface width */
    UINT16 width;
    /** \brief surface height */
    UINT16 height;
    /** \brief surface bitplane number */
    UINT16 depth;
    /** \brief TRUE if the data is interleaved */
    BOOL is_interleaved;
    /** \brief image data */
    void *buffer;
};

/**
 * A display buffer is a surface with a number. Since the display can
 * have any number of back buffers, it can be useful to know which one
 * is currently at the back an which one is at the front.
 */
struct Ratr0DisplayBuffer {
    struct Ratr0Surface surface;
    int buffernum;
};

/**
 * Shut down the display subsystem.
 */
extern void ratr0_display_shutdown(void);

/**
 * Initializes the display using the information in the init struct.
 * If the information is not equal to the existing one, the current
 * display buffer will be discarded and a new one will be built
 *
 * @param init_data the initialization struct
 */
extern void ratr0_display_init_buffers(struct Ratr0DisplayInit *init_data);

/**
 * Swap back buffer with the front buffer if available. Affects both playfields.
 */
extern void ratr0_display_swap_buffers(void);

//
// Quick access functions to the copper list.
//

/**
 * Sets the display palette. Allows to set the colors at an offset within the palette.
 *
 * @param colors array of colors
 * @param num_colors length of array
 * @param offset set the colors starting at this offset of the palette
 */
extern void ratr0_display_set_palette(UINT16 *colors, UINT8 num_colors, UINT8 offset);

struct Ratr0CopperListInfo {
    int ddfstrt_index, ddfstop_index, diwstrt_index, diwstop_index;
    int bplcon0_index, bpl1mod_index;
    int bpl1pth_index, spr0pth_index, color00_index;
};
extern struct Ratr0CopperListInfo DEFAULT_COPPER_INFO;

/**
 * Points the specified sprite to the image data.
 *
 * @param sprite_num sprite number
 * @param coplist pointer to copper list
 * @param size copper list size
 * @param info copper list index info
 * @param data pointer to sprite data structure
 */
extern void ratr0_display_set_sprite(UINT16 *coplist, int size,
                                     struct Ratr0CopperListInfo *info,
                                     int sprite_num, UINT16 *data);

/**
 * Initialize copper list with the current display values. The info structure
 * is used to say where the basic display definitions are located
 *
 * @param coplist pointer to copper list
 * @param num_words length of list in words
 * @param info copper list index info
 */
extern void ratr0_display_init_copper_list(UINT16 coplist[], int num_words,
                                           struct Ratr0CopperListInfo *info);

/**
 * Returns a pointer to the current front buffer.
 *
 * @param playfield_num the number of the playfield (0 or 1)
 * @return pointer to the current front buffer
 */
extern struct Ratr0DisplayBuffer *ratr0_display_get_front_buffer(UINT16 playfield_num);

/**
 * Returns a pointer to the current back buffer.
 *
 * @param playfield_num the number of the playfield (0 or 1)
 * @return pointer to the current back buffer
 */
extern struct Ratr0DisplayBuffer *ratr0_display_get_back_buffer(UINT16 playfield_num);

/**
 * Blits the specified surface to the front and back buffers. Can be
 * used to blit a background that will be restored using a custom mechanism.
 *
 * @param surface the Ratr0Surface to blit
 * @param playfield_num the number of the playfield (0 or 1)
 * @param dstx destination X coordinate
 * @param dsty destination Y coordinate
 */
extern BOOL ratr0_display_blit_surface_to_buffers(struct Ratr0Surface *surface,
                                                  UINT16 playfield_num,
                                                  UINT16 dstx, UINT16 dsty);

//
// Current front and back buffer numbers, these are made global for efficiency,
// never write directly and use with caution !
//
/** \brief index of the current back buffer */
//extern UINT16 ratr0_back_buffer;
/** \brief index of the current front buffer */
//extern UINT16 ratr0_front_buffer;

extern struct Ratr0CopperListInfo *current_copper_info;
extern UINT16 *current_coplist;
extern int current_coplist_size;

/**
 * Adds a dirty rectangle to the list at the specified position. The coordinates
 * are based on 16 pixel tiles rather than individual pixels.
 *
 * @param playfield_num the number of the playfield (0 or 1)
 * @param x coordinate of the dirty tile
 * @param y coordinate of the dirty tile
 */
extern void ratr0_display_add_dirty_rectangle(UINT16 playfield_num,
                                              UINT16 x, UINT16 y);

/**
 * Processes the dirty rectangle list of the current back buffer.
 *
 * @param process_dirty_rect a function that is called for every dirty rectangle
 */
extern void ratr0_display_process_dirty_rectangles(void (*process_dirty_rect)(struct Ratr0DisplayBuffer *display_buffer, UINT16 x, UINT16 y));

/**
 * \brief frame counter to show how many frames have elapsed since the last reset
 */
extern UINT8 frames_elapsed;

//
// Display Objects
//

/**
 * Collision box.
 * Collisions should usually be based on bounding shapes
 * that are tweaked for playability.
 */
struct Ratr0BoundingBox {
    /** \brief x-coordinate of origin */
    UINT16 x;
    /** \brief y-coordinate of origin */
    UINT16 y;
    /** \brief width of bounding box */
    UINT16 width;
    /** \brief height of bounding box */
    UINT16 height;
};

/** \brief maximum number of animation frames in a Ratr0AnimationFrames object */
#define RATR0_MAX_ANIM_FRAMES (8)
#define RATR0_ANIM_LOOP_TYPE_NONE (0)
#define RATR0_ANIM_LOOP_TYPE_LOOP (1)
#define RATR0_ANIM_LOOP_TYPE_PINGPONG (2)

/**
 * Visual component of an animated object. We keep it simple.
 *   - an animated sprite only represents a single animation state, if
 *     want more, group them, e.g. into a state pattern.
 *   - has an animation speed
 */
struct Ratr0AnimationFrames {
    /** \brief speed in frames */
    UINT8 speed;
    /** \brief frame numbers of the animation */
    UINT8 frames[RATR0_MAX_ANIM_FRAMES];
    /** \brief length of the frames array */
    UINT8 num_frames;
    /** \brief current animation frame index displayed */
    UINT8 current_frame_idx;
    /** \brief current tick, will reset to speed after reaching 0 */
    UINT8 current_tick;
    /** \brief indicates the loop type, 0=none, 1=loop, 2=pingpong */
    UINT8 loop_type;
    /** \brief indicates the loop direction, which can either be +1 or -1 */
    INT8 loop_dir;
};

/**
 * A translation object. Movement of objects
 * is implemented through this data structure. Never modify
 * an object's position by directly setting the bounds
 * object variables !!! The dirty rects algorithm relies on being
 * able to track position changes
 */
struct Ratr0Translate2D {
    /** \brief x translation */
    INT16 x;
    /** \brief y translation */
    INT16 y;
};

/**
 * The base data structure for animated objects. It is assumed that each such object
 * describes its boundary box, a collision box, a translation object and animation frames.
 * The data layout is deliberate, the collision box is the first element so we can
 * insert sprites into the spatial division data structure  and access the object
 * without any indirection.
 */
struct Ratr0Sprite {
    /** \brief collision boundaries */
    struct Ratr0BoundingBox collision_box;
    /** \brief Position and dimensions of the sprite, don't set directly !!! */
    struct Ratr0BoundingBox bounds;
    /** \brief Translation object to describe the next move */
    struct Ratr0Translate2D translate;
    /** \brief animation frames object */
    struct Ratr0AnimationFrames anim_frames;
};

/**
 * Base data structure for a static object. Static objects have a position
 * and can be collided with.
 * As with sprites, the collision box is the first data element since it
 * is added to the spatial division data structure.
 */
struct Ratr0StaticObject {
    /** \brief the collision box */
    struct Ratr0BoundingBox collision_box;
    /** \brief position and dimensions of the object  */
    struct Ratr0BoundingBox bounds;
};

/**
 * This is the background of the game. Backdrops are drawn to the display buffer in
 * their entirety the first time. After that, they serve as the source for restore
 * operations.
 */
struct Ratr0Backdrop {
    /** \brief surface object containing the image data  */
    struct Ratr0Surface surface;
    /** \brief flag to indicate whether the backdrop was drawn */
    BOOL was_drawn;
};

/**
 * Sprites have different specifications than
 * Blitter object, as they have a special data structure, we need
 * to have a representation that accomodates for that.
 * This structure also accounts for attached sprites in which case
 * every second frame in the data is the data for the second sprite
 * in the attached sprite
 */
struct Ratr0HWSprite {
    /** \brief base node data */
    struct Ratr0Sprite base_obj;

    /** \brief whether the sprite is attached */
    BOOL is_attached;

    /** \brief sprite image data in Amiga sprite format */
    UINT16 *sprite_data;
};

extern struct Ratr0HWSprite NULL_HW_SPRITE;

/**
 * Representation of a BOB.
 */
struct Ratr0Bob {
    /** \brief base node data */
    struct Ratr0Sprite base_obj;
    /** \brief BOB image data, stored in a tile sheet */
    struct Ratr0TileSheet *tilesheet;
};

struct Ratr0TileSheet;
struct Ratr0SpriteSheet;

/**
 * Create an Amiga hardware sprite object from a tilesheet.
 *
 * @param tilesheet pointer to tilesheet containing the image data
 * @param frames array containing the frames of the animation
 * @param num_frames length of the frames array
 * @param speed animation speed in frames
 * @return pointer to an initialized sprite data structure
 */
extern struct Ratr0HWSprite *ratr0_create_sprite(struct Ratr0TileSheet *tilesheet,
                                                 UINT8 frames[], UINT8 num_frames,
                                                 UINT8 speed);

/**
 * Create a HW sprite object from a sprite sheet.
 *
 * @param sheet sprite sheet pointer
 * @param speed animation speed
 * @param loop_type loop type
 * @return pointer to an animated HW sprite
 */
extern struct Ratr0HWSprite *ratr0_create_sprite_from_sprite_sheet(struct Ratr0SpriteSheet *sheet,
                                                                   UINT8 speed,
                                                                   UINT8 loop_type);

/**
 * Extract a non-animated sprite from a sprite sheet frame.
 *
 * @param sheet sprite sheet pointer
 * @param framenum frame number
 * @return pointer to a static HW sprite
 */
extern struct Ratr0HWSprite *ratr0_create_sprite_from_sprite_sheet_frame(struct Ratr0SpriteSheet *sheet, int framenum);

/**
 * Create a blitter object from a tile sheet.
 *
 * @param tilesheet pointer to tilesheet containing the image data
 * @param frames array containing the frames of the animation
 * @param num_frames length of the frames array
 * @param speed animation speed in frames
 * @return pointer to an initialized BOB data structure
 */
extern struct Ratr0Bob *ratr0_create_bob(struct Ratr0TileSheet *tilesheet,
                                         UINT8 frames[], UINT8 num_frames,
                                         UINT8 speed);

/**
 * Debug helper function to write a copper list to a C source file.
 *
 * @param copperlist the copperlist to write
 * @param len number of words in the copper list
 * @param path path to the output file
 */
extern void ratr0_dump_copperlist(UINT16 *copperlist, int len, const char *path);

/**
 * Sets a new copper list to the display.
 *
 * @param copperlist pointer to new copper list
 * @param size number of words in the copper list
 * @param info description of the copper list
 */
extern void ratr0_display_set_copperlist(UINT16 *copperlist, int size,
                                         struct Ratr0CopperListInfo *info);

#endif /* __RATR0_DISPLAY_H__ */
