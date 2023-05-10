/** @file input.h
 *
 * Amiga specific Input subsystem.
 */
#pragma once
#ifndef __RATR0_AMIGA_INPUT_H__
#define __RATR0_AMIGA_INPUT_H__

/**
 * Start up the input subsystem.
 */
extern void ratr0_amiga_input_startup(void);

/**
 * Shut down the input subsystem.
 */
extern void ratr0_amiga_input_shutdown(void);

/**
 * Updates the state of the current user input.
 */
extern void ratr0_amiga_input_update(void);

/**
 * Retrieves the joystick state.
 *
 * @param device_num joystick number
 * @return joystick state encoded into an integer
 */
extern UINT32 ratr0_amiga_get_joystick_state(UINT16 device_num);

#endif /* __RATR0_AMIGA_INPUT_H__ */
