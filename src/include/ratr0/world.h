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

/**
 * A scene is a component of a game. It contains the movable and static game objects
 * and the assets. The game can also provide functions for transitions and scene specific
 * updates
 */
struct Ratr0Scene {
    Ratr0Engine *engine;

    /**
     * A hierarchy of child nodes that are used to organize the scene.
     * Theoretically, we don't need this and can do everything in the
     * scene object.
     */
    struct Ratr0Node *children;

    /**
     * A scene can have a backdrop, if it does not need a tile map, this might
     * be the only thing you need. Can be null if you don't need a backdrop.
     */
    struct Ratr0Backdrop *backdrop;
    /**
     * A number of optional tilemaps, which can define a screen. Can be null if
     * you don't need any level maps
     */
    struct Ratr0Node *tilemaps;

    /**
     * The animated objects in the scene that are visible/active. The world module will
     * automatically render objects in these lists.
     * On Amiga, these are both sprites and BOBs, and we keep these separate
     * so we won't need any type checks.
     */
    struct Ratr0AnimatedAmigaBob *bobs;
    struct Ratr0AnimatedAmigaSprite *sprites;

    void (*on_enter)(struct Ratr0Scene *this_scene);
    void (*on_exit)(struct Ratr0Scene *this_scene);
    void (*update)(struct Ratr0Scene *this_scene, UINT8 frames_elapsed);
};

/**
 * This interface serves as the creator of our scene objects.
 */

struct Ratr0NodeFactory {
    struct Ratr0Scene *(*create_scene)(void);
    struct Ratr0AnimatedSprite *(*create_sprite)(struct Ratr0TileSheet *tilesheet,
                                                 UINT8 frames[], UINT8 num_frames,
                                                 UINT8 speed, BOOL is_hw);
    struct Ratr0Backdrop *(*create_backdrop)(struct Ratr0TileSheet *tilesheet);
};

struct Ratr0WorldSystem {
    void (*set_current_scene)(struct Ratr0Scene *);
    void (*update)(UINT8 frames_elapsed);
    void (*shutdown)(void);

    struct Ratr0NodeFactory *(*get_node_factory)(void);
    void (*add_child)(struct Ratr0Node *parent, struct Ratr0Node *child);

    // a method that is called every frame
    //void (*update_node)(struct Ratr0Node *);
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
