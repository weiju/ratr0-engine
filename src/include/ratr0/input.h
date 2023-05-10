/** @file input.h
 *
 * Input subsystem.
 */
#pragma once
#ifndef __RATR0_INPUT_H__
#define __RATR0_INPUT_H__
#include <ratr0/data_types.h>
#include <ratr0/engine.h>

/** \brief the supported set of keyboard keys */
enum PhysicalKeys {
    PHYS_KEY_ESCAPE, PHYS_KEY_SPACE, PHYS_KEY_UP, PHYS_KEY_DOWN, PHYS_KEY_LEFT, PHYS_KEY_RIGHT
};

// 32 bit flags for joystick/mouse state

/** \brief fire 0 button on joystick */
#define JOY_FIRE0   (1)
/** \brief digital joystick left */
#define JOY_D_LEFT  (2)
/** \brief digital joystick right */
#define JOY_D_RIGHT (4)
/** \brief digital joystick up */
#define JOY_D_UP    (8)
/** \brief digital joystick down */
#define JOY_D_DOWN  (16)

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
     * Returns the joystick state.
     *
     * @param joystick number
     * @return the joystick state encoded in an integer number
     */
    UINT32 (*get_joystick_state)(UINT16 device_num);
};

/**
 * Start up the input subsystem.
 *
 * @param engine pointer to Ratr0Engine instance
 * @return pointer to initialized Ratr0InputSystem
 */
extern struct Ratr0InputSystem *ratr0_input_startup(Ratr0Engine *engine);

#endif /* __RATR0_INPUT_H__ */
