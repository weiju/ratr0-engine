/** @file input.h
 *
 * Amiga specific Input module. The functions in this module are used by
 * the input subsystem to poll system specific input per frame.
 */
#pragma once
#ifndef __RATR0_AMIGA_INPUT_H__
#define __RATR0_AMIGA_INPUT_H__

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
 * Start up the input module.
 */
extern void ratr0_amiga_input_startup(void);

/**
 * Shut down the Amiga input module.
 */
extern void ratr0_amiga_input_shutdown(void);

/**
 * Retrieves the joystick state.
 *
 * @param device_num joystick number
 * @return joystick state encoded into an integer
 */
extern UINT16 ratr0_amiga_get_joystick_state(UINT8 device_num);

extern UINT16 ratr0_amiga_get_keyboard_state(void);

#endif /* __RATR0_AMIGA_INPUT_H__ */
