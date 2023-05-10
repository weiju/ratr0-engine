#pragma once
#ifndef __RATR0_AMIGA_DISPLAY_H__
#define __RATR0_AMIGA_DISPLAY_H__
#include <ratr0/data_types.h>
#include <ratr0/engine.h>

/* Amiga Display subsystem */
/*
  Amiga specific information about the display, used both for
  initialization and query information. This includes aspects of the
  playfield hardware, sprites and the blitter.
*/
struct Ratr0AmigaDisplayInfo {
    /*
      Width is a multiple of 16 and should be <= 320. Height can be max 200 for NTSC and
      256 for PAL. Smaller values will typically result in less memory consumption and faster
      refresh times.
      Sensible values for width can be { 320, 288 }
      Sensible values for height can be { 192, 208, 224, 240 }
    */
    UINT16 vp_width, vp_height;
    UINT16 buffer_width, buffer_height;

    /* This can be a value between 1 and 5 */
    UINT8 depth;

    /* Display buffer */
    UINT8 num_buffers;

    /* How many frames to update the backbuffer ? For now, this
     * should only be either 1 or 2. More than that heavily impacts
     * gameplay experience.
     */
    UINT8 update_frames;

    /* Readonly section */
    /* This is the Amiga relevant part. We can't explicitly set
       PAL or NTSC since it is dependent on the machine, so it is query
       only */
    BOOL is_pal;
};

/**
 * Information about the current display in this object.
 * We can use this for both playfield hardware setup and blitter setup.
 * In the future we can use this for backbuffer and/or dual playfield
 * information.
 */
struct Ratr0AmigaSurface {
    UINT16 width, height, depth;
    BOOL is_interleaved;
    void *buffer;
};

/**
 * Start up the display subsystem.
 */
extern void ratr0_amiga_display_startup(Ratr0Engine *, struct Ratr0RenderingSystem *,
                                        struct Ratr0DisplayInfo *);

/**
 * Shut down the display subsystem.
 */
extern void ratr0_amiga_display_shutdown(void);

/**
 * Wait for vertical blank
 */
extern void ratr0_amiga_wait_vblank(void);

extern void ratr0_amiga_display_swap_buffers(void);

/**
 * Process all queued up render commands.
 */
extern void ratr0_amiga_display_update(void);

/**
 * Quick access functions to the copper list.
 */
/**
 * Adjust the copper list to point to the specified display buffer with the
 * given specification.
 * The data in the display buffer is assumed to be interleaved.
 */
extern struct Ratr0AmigaSurface *ratr0_amiga_get_front_buffer(void);
extern struct Ratr0AmigaSurface *ratr0_amiga_get_back_buffer(void);

extern void ratr0_amiga_set_palette(UINT16 *colors, UINT8 num_colors);
extern void ratr0_amiga_display_set_sprite(int sprite_num, UINT16 *data);

extern void add_dirty_rectangle(UINT16 x, UINT16 y);
extern void process_dirty_rectangles(void (*process_dirty_rect)(UINT16 x, UINT16 y));

/**
 * Current front and back buffer numbers, these are made global for efficiency,
 * never write directly and use with caution !
 */
extern UINT16 ratr0_amiga_back_buffer;
extern UINT16 ratr0_amiga_front_buffer;

/**
 * The frame counter to show how many frames have elapsed since the last
 * reset.
 */
extern UINT8 frames_elapsed;

/**
 * Display Objects
 */

/*
 * Collision box.
 * Collisions should usually be based on bounding shapes
 * that are tweaked for playability.
 */
struct Ratr0BoundingBox {
    UINT16 x, y, width, height;
};

/*
 * Visual component of an animated object. We keep it simple.
 *   - an animated sprite only represents a single animation state, if
 *     want more, group them, e.g. into a state pattern.
 *   - has an animation speed
 */
#define RATR0_MAX_ANIM_FRAMES (8)

struct Ratr0AnimationFrames {
    UINT8 speed;  // speed in frames
    UINT8 frames[RATR0_MAX_ANIM_FRAMES];
    UINT8 num_frames; // number of frames in animation
    UINT8 current_frame_idx; // current animation frame index displayed
    UINT8 current_tick;  // current tick, will reset to speed after reaching 0
    BOOL  is_looping;  // indicates whether this is a looping animation
};

struct Ratr0Translate2D {
    UINT16 x, y;
};

struct Ratr0AnimatedSprite {
    struct Ratr0Node node;
    struct Ratr0AnimationFrames anim_frames;

    // Position and dimensions, read-only
    struct Ratr0BoundingBox bounds;

    // Translation in this frame. Movement of objects
    // is implemented through this object. Never modify
    // an object's position by directly setting the bounds
    // object !!! The dirty rects algorithm relies on being
    // able to track if position changes
    struct Ratr0Translate2D translate;

    // collision boundaries
    struct Ratr0BoundingBox collision_box;
};

/**
 * This is the background of the game.
 */
struct Ratr0Backdrop {
    struct Ratr0AmigaSurface surface;
    BOOL was_drawn;
};

/**
 * Sprites have different specifications than
 * Blitter object, as they have a special data structure, we need
 * to have a representation that accomodates for that.
 */
struct Ratr0AnimatedAmigaSprite {
    struct Ratr0AnimatedSprite base_obj;  // inherited base members
    UINT16 *sprite_data;
};

struct Ratr0AnimatedAmigaBob {
    struct Ratr0AnimatedSprite base_obj;  // inherited base members
    struct Ratr0TileSheet *tilesheet;
};

extern struct Ratr0AnimatedAmigaSprite *ratr0_create_amiga_sprite(struct Ratr0TileSheet *tilesheet,
                                                                  UINT8 frames[], UINT8 num_frames,
                                                                  UINT8 speed);

/**
 * Create a blitter object from a tile sheet.
 */
extern struct Ratr0AnimatedAmigaBob *ratr0_create_amiga_bob(struct Ratr0TileSheet *tilesheet,
                                                            UINT8 frames[], UINT8 num_frames,
                                                            UINT8 speed);


#endif /* __RATR0_AMIGA_DISPLAY_H__ */
