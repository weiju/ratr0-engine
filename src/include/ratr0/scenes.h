#pragma once
#ifndef __RATR0_SCENES_H__
#define __RATR0_SCENES_H__
#include <ratr0/data_types.h>
#include <ratr0/engine.h>
#include <ratr0/resources.h>

/* Scenes subsystem */

/*
 * Top level node that is the base of a node. The node system is inspired by
 * the Godot design, but will be much simpler to accomodate to the target systems
 * that are very memory constrained.
 * In general, nodes can call methods on nodes that are below them in the scene tree,
 * while signal are used to communicate up and across the hierarchy.
 */
struct Ratr0Node {
    UINT32 id;  // a unique identifier
    struct Ratr0Node *next, *prev, *children;

    // a method that is called every frame
    void (*update)(void);
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
struct Ratr0AnimatedSprite2D {
    struct Ratr0Node node;  // include node properties

    UINT16 x,y;
    UINT8 speed;  // speed in frames
    UINT8 num_frames; // number of frames in animation
    UINT8 current_frame; // current animation frame displayed
    UINT8 current_tick;  // current tick, will reset to speed after reaching 0
    BOOL  is_looping;  // indicates whether this is a looping animation

    void (*display)(void);  // called to display this object on the screen

    // collision boundaries
    struct Ratr0CollisionBox collision_box;
};

/**
 * This interface serves as the creator of our scene objects.
 */
struct Ratr0NodeFactory {
    struct Ratr0AnimatedSprite2D (*create_animated_sprite)(void);
};

struct Ratr0SceneSystem {
    void (*set_current_scene)(struct Ratr0Node *);
    void (*update)(void);
    void (*shutdown)(void);

    struct Ratr0NodeFactory *(*get_node_factory)(void);
};

/**
 * Start up the scene subsystem.
 */
extern struct Ratr0SceneSystem *ratr0_scenes_startup(Ratr0Engine *);

#endif /* __RATR0_SCENES_H__ */
