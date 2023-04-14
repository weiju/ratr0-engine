#pragma once
#ifndef __RATR0_INPUT_H__
#define __RATR0_INPUT_H__
#include <ratr0/data_types.h>
#include <ratr0/engine.h>

/**
 * Check for keys.
 */
enum PhysicalKeys {
    PHYS_KEY_ESCAPE, PHYS_KEY_SPACE, PHYS_KEY_UP, PHYS_KEY_DOWN, PHYS_KEY_LEFT, PHYS_KEY_RIGHT
};


/* 32 bit flags for joystick/mouse state */
#define JOY_FIRE0   (1)
#define JOY_D_LEFT  (2)
#define JOY_D_RIGHT (4)
#define JOY_D_UP    (8)
#define JOY_D_DOWN  (16)

/*
 * Input subsystem. This abstracts system specific input events into.
 */
struct Ratr0InputSystem {
    void (*shutdown)(void);
    void (*update)(void);
    UINT32 (*get_joystick_state)(UINT16 device_num);
};

/**
 * Start up the input subsystem.
 */
extern struct Ratr0InputSystem *ratr0_input_startup(Ratr0Engine *);


#endif /* __RATR0_INPUT_H__ */
