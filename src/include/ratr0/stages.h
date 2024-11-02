/** @file stages.h
 *
 * This is RATR0 Engine's implementation of the Stages subsystem.
 */
#pragma once
#ifndef __RATR0_STAGES_H__
#define __RATR0_STAGES_H__
#include <ratr0/data_types.h>
#include <ratr0/engine.h>
#include <ratr0/resources.h>
#include <ratr0/display.h>


// just to make the compiler happy
struct Ratr0Stage;

/**
 * A stage is a component of a game. It contains the movable and static game
 * objects and the assets. The game can also provide functions for transitions
 * and stage-specific updates.
 */
struct Ratr0Stage {

    /** \brief pointer to engine instance */
    Ratr0Engine *engine;

    /** \brief memory handle to this stage's copper list */
    Ratr0MemHandle h_copper_list;

    /** \brief quick pointer to this stage's copper list */
    UINT16 *copper_list;

    /**
     * \brief this stage's backdrop object
     *
     * A stage can have a backdrop, if it does not need a tile map, this might
     * be the only thing you need. Can be null if you don't need a backdrop.
     * The backdrop is essentially the restore buffer. If all you need is
     * a backdrop this is easy, because BOBs will also just work.
     * If you need a scrolling level or manage your own restore
     * method, don't use the backdrop and instead just go and use
     * the engine's front and back buffers directly
     */
    struct Ratr0Backdrop *backdrop;

    //
    // The animated objects in the stage that are visible/active. The stages
    // module will automatically render objects in these lists.
    // On Amiga, these are both sprites and BOBs, and we keep these separate
    // so we won't need any type checks.
    //
    /** \brief list of active BOBs in the stage */
    struct Ratr0Bob *bobs[10];

    /** \brief number of bobs in the array */
    int num_bobs;

    /** \brief list of active hardware sprites in the stage */
    struct Ratr0HWSprite *sprites[8];

    /** \brief number of sprites in the array */
    int num_sprites;

    /**
     * Adds a bob to the stage.
     *
     * @param this_stage pointer to this stage
     * @param bob the BOB to add to the stage
     */
    void (*add_bob)(struct Ratr0Stage *this_stage, struct Ratr0Bob *bob);

    /**
     * User provided function that is called when this stage is set to
     * the current stage.
     *
     * @param this_stage pointer to this stage
     */
    void (*on_enter)(struct Ratr0Stage *this_stage);

    /**
     * User provided function that is called when the current stage changes to
     * a different stage.
     *
     * @param this_stage pointer to this stage
     */
    void (*on_exit)(struct Ratr0Stage *this_stage);

    /**
     * User provided function that is called on every frame of the game loop
     * while this stage is the current active stage.
     *
     * @param this_stage pointer to this stage
     * @param backbuffer pointer to the display back buffer
     * @param frames_elapsed the number of elapsed frames since the last call of update
     */
    void (*update)(struct Ratr0Stage *this_stage,
                   struct Ratr0DisplayBuffer *backbuffer,
                   UINT8 frames_elapsed);

};

/**
 * This interface serves as the creator of our stage objects.
 */
struct Ratr0NodeFactory {
    /**
     * Creates a new Ratr0Stage object.
     *
     * @return pointer to an initialized Ratr0Stage object
     */
    struct Ratr0Stage *(*create_stage)(void);

    // TODO:
    // -----
    // This should actually be funcitons of the resources subsystem.
    // Logically this does not belong here. Also, the sprites and
    // backdrops are not associated with a stage. This is a design
    // flaw
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
    struct Ratr0Sprite *(*create_sprite)(struct Ratr0TileSheet *tilesheet,
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
 * Interface to the Stages subsystem.
 */
struct Ratr0StagesSystem {
    /**
     * Sets the currently active stage.
     *
     * @param stage the stage to set
     */
    void (*set_current_stage)(struct Ratr0Stage *stage);

    /**
     * Called every game loop iteration to update the Stages system.
     *
     * @param backbuffer the backbuffer of the display
     * @param frames_elapsed frames elapsed since last invocation
     */
    void (*update)(struct Ratr0DisplayBuffer *backbuffer, UINT8 frames_elapsed);

    /**
     * Shutdown the Stages subsystem.
     */
    void (*shutdown)(void);

    /**
     * Retrieve the singleton node factory instance.
     *
     * @return the singleton node factory instance
     */
    struct Ratr0NodeFactory *(*get_node_factory)(void);
};

/**
 * Start up the stage subsystem.
 *
 * @param eng the engine object
 * @return an initialized Ratr0StagesSystem instance
 */
extern struct Ratr0StagesSystem *ratr0_stages_startup(Ratr0Engine *eng);

#endif /* __RATR0_STAGES_H__ */
