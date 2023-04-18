#pragma once
#ifndef __RATR0_WORLD_H__
#define __RATR0_WORLD_H__
#include <ratr0/data_types.h>
#include <ratr0/engine.h>
#include <ratr0/resources.h>
#ifdef AMIGA
#include <ratr0/amiga/display.h>
#endif

/* World subsystem */

enum _Ratr0NodeClassIDs {
    RATR0_NODE = 1, BACKDROP, ANIM_SPRITE2D, AMIGA_SPRITE, AMIGA_BOB
};

/*
 * Top level node that is the base of a node. The node system is inspired by
 * the Godot design, but will be much simpler to accomodate to the target systems
 * that are very memory constrained.
 * In general, nodes can call methods on nodes that are below them in the scene tree,
 * while signal are used to communicate up and across the hierarchy.
 */
struct Ratr0Node {
    /* Identifying information. We don't support subclassing. It's rather a way to find the
     * appropriate handlers */
    UINT16 class_id;

    struct Ratr0Node *next, *children;
};

/*
 * Collision box.
 * Collisions should usually be based on bounding shapes
 * that are tweaked for playability.
 */
struct Ratr0CollisionBox {
    UINT16 x, y, width, height;
};

/*
 * Visual component of an animated object. We keep it simple.
 *   - an animated sprite only represents a single animation state, if
 *     want more, group them, e.g. into a state pattern.
 *   - has an animation speed
 */
struct Ratr0AnimatedSprite {
    struct Ratr0Node node;  // include node properties
    // next in render queue
    struct Ratr0AnimatedSprite *next;

    UINT16 x,y,zindex;
    UINT8 speed;  // speed in frames
    UINT8 num_frames; // number of frames in animation
    UINT8 current_frame; // current animation frame displayed
    UINT8 current_tick;  // current tick, will reset to speed after reaching 0
    BOOL  is_looping;  // indicates whether this is a looping animation

    // collision boundaries
    struct Ratr0CollisionBox collision_box;
};

/**
 * This is the background of the game.
 */
struct Ratr0Backdrop {
    struct Ratr0Node node;  // include node properties
#ifdef AMIGA
    struct Ratr0AmigaSurface surface;
    BOOL was_drawn;
#endif
};

/**
 * This interface serves as the creator of our scene objects.
 */
struct Ratr0NodeFactory {
    struct Ratr0Node *(*create_node)(void);
    struct Ratr0AnimatedSprite *(*create_animated_sprite)(struct Ratr0TileSheet *tilesheet,
                                                          UINT8 *frame_indexes, UINT8 num_indexes,
                                                          BOOL is_hw);
    struct Ratr0Backdrop *(*create_backdrop)(struct Ratr0TileSheet *tilesheet);
};

struct Ratr0WorldSystem {
    void (*set_current_scene)(struct Ratr0Node *);
    void (*update)(void);
    void (*shutdown)(void);

    struct Ratr0NodeFactory *(*get_node_factory)(void);
    void (*add_child)(struct Ratr0Node *parent, struct Ratr0Node *child);

    // a method that is called every frame
    void (*update_node)(struct Ratr0Node *);
};

/**
 * Start up the scene subsystem.
 */
extern struct Ratr0WorldSystem *ratr0_world_startup(Ratr0Engine *);

/**
 * Base node initialization.
 */
extern void ratr0_world_init_base_node(struct Ratr0Node *node, UINT16 clsid);

#endif /* __RATR0_WORLD_H__ */
