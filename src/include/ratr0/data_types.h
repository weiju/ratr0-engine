/** @file data_types.h */
#pragma once
#ifndef __RATR0_DATA_TYPES_H__
#define __RATR0_DATA_TYPES_H__

/*
 * RATR0 Engine basic data types. Makes sure we are operating on the
 * same size of data independent of system and compiler.
 */

#ifdef __VBCC__
/*
 * We only use the BOOL definitions from here, while other primitives use
 * a more generic name.
 */
#include <exec/types.h>
/**
 * \var typedef int INT32
 * \brief A type definition for signed 32 bit numbers.
 */
typedef int INT32;
/**
 * \var typedef unsigned int UINT32
 * \brief A type definition for unsigned 32 bit numbers.
 */
typedef unsigned int UINT32;
/**
 * \var typedef short INT16
 * \brief A type definition for signed 16 bit numbers.
 */
typedef short INT16;
/**
 * \var typedef unsigned short UINT16
 * \brief A type definition for unsigned 16 bit numbers.
 */
typedef unsigned short UINT16;
/**
 * \var typedef char INT8
 * \brief A type definition for signed 8 bit numbers.
 */
typedef char INT8;
/**
 * \var typedef unsigned char UINT8
 * \brief A type definition for unsigned 8 bit numbers.
 */
typedef unsigned char UINT8;

/**
 * \var typedef char CHAR
 * \brief A type definition for 8 bit characters.
 */
typedef char CHAR;

#else

#include <stdint.h>

/**
 * \var typedef int32_t INT32
 * \brief A type definition for signed 32 bit numbers.
 */
typedef int32_t INT32;
/**
 * \var typedef uint32_t UINT32
 * \brief A type definition for unsigned 32 bit numbers.
 */
typedef uint32_t UINT32;
typedef int16_t INT16;
typedef uint16_t UINT16;
typedef int8_t INT8;
typedef uint8_t UINT8;

typedef char CHAR;
typedef int16_t BOOL;

#define TRUE (1)
#define FALSE (0)
#ifndef NULL
#define NULL (0)
#endif

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN
#endif /* LITTLE_ENDIAN */

#endif /* VBCC */

// just to make the compiler happy
struct Ratr0Scene;

/**
 * Nodes are the base object of the system
 * Top level node that is the base of a node. The node system is inspired by
 * the Godot design, but will be much simpler to accomodate to the target systems
 * that are very memory constrained.
 * In general, nodes can call methods on nodes that are below them in the scene tree,
 * while signal are used to communicate up and across the hierarchy.
 */
struct Ratr0Node {
    /**
     * \brief node class identifier
     *
     * Identifying information. We don't support subclassing. It's rather a way to find the
     * appropriate handlers
     */
    UINT16 class_id;

    /** \brief next sibling node */
    struct Ratr0Node *next;
    /** \brief first child node */
    struct Ratr0Node *children;

    /**
     * The node's update function. This is a way to customize system behavior
     * It is optional, if the update function set to a non-null, value, it
     * will be executed on each iteration of the game loop.
     *
     * @param scene the scene that contains this node
     * @param this_node the node to run the update function on
     */
    void (*update)(struct Ratr0Scene *scene, struct Ratr0Node *this_node);
};

/**
 * \brief Built-in node types in the RATR0 engine.
 */
enum Ratr0NodeTypes { RATR0_NODE, BACKGROUND, ANIM_SPRITE2D, AMIGA_SPRITE, AMIGA_BOB};


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
    /** \brief indicates whether this is a looping animation */
    BOOL  is_looping;
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
 */
struct Ratr0AnimatedSprite {
    /** \brief Position and dimensions of the sprite, don't set directly !!! */
    struct Ratr0BoundingBox bounds;
    /** \brief collision boundaries */
    struct Ratr0BoundingBox collision_box;
    /** \brief Translation object to describe the next move */
    struct Ratr0Translate2D translate;
    /** \brief animation frames object */
    struct Ratr0AnimationFrames anim_frames;
};

#endif /* __RATR0_DATA_TYPES_H__ */
