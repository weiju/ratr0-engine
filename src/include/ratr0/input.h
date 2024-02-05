/** @file input.h
 *
 * Input subsystem.
 */
#pragma once
#ifndef __RATR0_INPUT_H__
#define __RATR0_INPUT_H__
#include <ratr0/data_types.h>
#include <ratr0/engine.h>

/**
 * \var typedef INT16 RATR0_ACTION_ID
 * \brief A type definition for input action ids.
 */
typedef INT16 RATR0_ACTION_ID;

/** \brief available input classes */
enum Ratr0InputClasses {
    RATR0_IC_KB = 0, RATR0_IC_JS0, RATR0_IC_JS1, RATR0_IC_MOUSE
};

/** \brief number of input classes */
#define RATR0_NUM_INPUT_CLASSES (RATR0_IC_MOUSE - RATR0_IC_KB)

/** \brief number of input ids per class */
#define RATR0_NUM_INPUT_IDS (10)

/** \brief the supported set of keyboard keys */
enum Ratr0PhysicalKeys {
    RATR0_KEY_NONE = 0, RATR0_KEY_ESCAPE, RATR0_KEY_SPACE, RATR0_KEY_UP, RATR0_KEY_DOWN,
    RATR0_KEY_LEFT, RATR0_KEY_RIGHT
};

/** \brief available joystick input actions */
enum Ratr0JoystickInputs {
    RATR0_INPUT_JS_LEFT = 0, RATR0_INPUT_JS_RIGHT, RATR0_INPUT_JS_UP, RATR0_INPUT_JS_DOWN,
    RATR0_INPUT_JS_BUTTON0
};

/**
 * Abstraction for an input event.
 */
struct Ratr0InputEvent {
    /** \brief input class */
    UINT16 input_class;
    /** \brief input id within the class */
    UINT16 input_id;

    /** \brief mapped action id */
    RATR0_ACTION_ID action_id;

    /** \brief next input event in the list */
    struct Ratr0InputEvent *next;
};

/**
 * Input subsystem. This abstracts system specific input events into actions.
 */
struct Ratr0InputSystem {
    /**
     * Shuts down the input subsystem.
     */
    void (*shutdown)(void);
    /**
     * Update the input system state.
     */
    void (*update)(void);

    /**
     * Allocate an action.
     *
     * @return a new action id
     */
    RATR0_ACTION_ID (*alloc_action)(void);

    /**
     * Maps an input to an action. Multiple inputs can be mapped to an action.
     *
     * @param action_id the action id
     * @param input_class input class
     * @param input_id input id
     */
    void (*map_input_to_action)(RATR0_ACTION_ID action_id, UINT16 input_class, UINT16 input_id);

    /**
     * Returns TRUE if the specified action was pressed in the current loop iteration.
     * Multiple actions can  be active in a single loop iteration.
     *
     * @param action_id action id
     * @return TRUE if action was pressed, FALSE otherwise
     */
    BOOL (*was_action_pressed)(RATR0_ACTION_ID action_id);
};

/**
 * Start up the input subsystem.
 *
 * @param engine pointer to Ratr0Engine instance
 * @return pointer to initialized Ratr0InputSystem
 */
extern struct Ratr0InputSystem *ratr0_input_startup(Ratr0Engine *engine);

#endif /* __RATR0_INPUT_H__ */
