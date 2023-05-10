/** @file world.h
 *
 * This is RATR0 Engine's implementation of the World subsystem.
 */
#pragma once
#ifndef __RATR0_WORLD_H__
#define __RATR0_WORLD_H__
#include <ratr0/data_types.h>
#include <ratr0/engine.h>
#include <ratr0/resources.h>
#ifdef AMIGA
#include <ratr0/amiga/display.h>
#endif

/**
 * A scene is a component of a game. It contains the movable and static game objects
 * and the assets. The game can also provide functions for transitions and scene specific
 * updates
 */
struct Ratr0Scene {

    /** \brief pointer to engine instance */
    Ratr0Engine *engine;

    /**
     * \brief child nodes of the scene
     *
     * A hierarchy of child nodes that are used to organize the scene.
     * Theoretically, we don't need this and can do everything in the
     * scene object.
     */
    struct Ratr0Node *children;

    /**
     * \brief this scene's backdrop object
     *
     * A scene can have a backdrop, if it does not need a tile map, this might
     * be the only thing you need. Can be null if you don't need a backdrop.
     */
    struct Ratr0Backdrop *backdrop;
    /**
     * A number of optional tilemaps, which can define a screen. Can be null if
     * you don't need any level maps
     */
    struct Ratr0Node *tilemaps;

    //
    // The animated objects in the scene that are visible/active. The world module will
    // automatically render objects in these lists.
    // On Amiga, these are both sprites and BOBs, and we keep these separate
    // so we won't need any type checks.
    //
    /** \brief list of active BOBs in the scene */
    struct Ratr0AnimatedAmigaBob *bobs;
    /** \brief list of active hardware sprites in the scene */
    struct Ratr0AnimatedAmigaSprite *sprites;

    /**
     * User provided function that is called when this scene is set to the current scene.
     *
     * @param this_scene pointer to this scene
     */
    void (*on_enter)(struct Ratr0Scene *this_scene);

    /**
     * User provided function that is called when the current scene changes to a different scene.
     *
     * @param this_scene pointer to this scene
     */
    void (*on_exit)(struct Ratr0Scene *this_scene);

    /**
     * User provided function that is called on every frame of the game loop while this scene
     * is the current active scene.
     *
     * @param this_scene pointer to this scene
     * @param frames_elapsed the number of elapsed frames since the last call of update
     */
    void (*update)(struct Ratr0Scene *this_scene, UINT8 frames_elapsed);
};

/**
 * This interface serves as the creator of our scene objects.
 */
struct Ratr0NodeFactory {
    /**
     * Creates a new Ratr0Scene object.
     *
     * @return pointer to an initialized Ratr0Scene object
     */
    struct Ratr0Scene *(*create_scene)(void);

    /**
     * Creates a new sprite.
     *
     * @param tilesheet pointer to a tilesheet
     * @param frames array of frames within the tilesheet that defines the animation
     * @param num_frames length of the frames array
     * @param speed speed of the animation in frames
     * @param is_hw if TRUE, a hardware sprite is created, otherwise a BOB
     * @return pointer to an initialized sprite
     */
    struct Ratr0AnimatedSprite *(*create_sprite)(struct Ratr0TileSheet *tilesheet,
                                                 UINT8 frames[], UINT8 num_frames,
                                                 UINT8 speed, BOOL is_hw);

    /**
     * Creates a new backdrop.
     *
     * @param tilesheet pointer to a tilesheet
     * @return pointer to an initialized backdrop
     */
    struct Ratr0Backdrop *(*create_backdrop)(struct Ratr0TileSheet *tilesheet);
};


/**
 * Interface to the World subsystem.
 */
struct Ratr0WorldSystem {
    /**
     * Sets the currently active scene.
     *
     * @param scene the scene to set
     */
    void (*set_current_scene)(struct Ratr0Scene *scene);

    /**
     * Called every game loop iteration to update the World system.
     *
     * @param frames_elapsed frames elapsed since last invocation
     */
    void (*update)(UINT8 frames_elapsed);

    /**
     * Shutdown the World subsystem.
     */
    void (*shutdown)(void);

    /**
     * Retrieve the singleton node factory instance.
     *
     * @return the singleton node factory instance
     */
    struct Ratr0NodeFactory *(*get_node_factory)(void);

    /**
     * Adds a child node to another node.
     *
     * @param parent the parent node
     * @param child the new child node
     */
    void (*add_child)(struct Ratr0Node *parent, struct Ratr0Node *child);
};

/**
 * Start up the scene subsystem.
 *
 * @param eng the engine object
 * @return an initialized Ratr0WorldSystem instance
 */
extern struct Ratr0WorldSystem *ratr0_world_startup(Ratr0Engine *eng);

/**
 * Base node initialization. Sets a Ratr0Node struct members to a defined state.
 *
 * @param node pointer to the node to initialize
 * @param clsid the class id
 */
extern void ratr0_world_init_base_node(struct Ratr0Node *node, UINT16 clsid);

#endif /* __RATR0_WORLD_H__ */
