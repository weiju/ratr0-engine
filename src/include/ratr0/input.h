#pragma once
#ifndef __RATR0_INPUT_H__
#define __RATR0_INPUT_H__

/* Input subsystem */
struct Ratr0InputSystem {
    void (*shutdown)(void);
};

/**
 * Start up the input subsystem.
 */
extern struct Ratr0InputSystem *ratr0_input_startup(void);


#endif /* __RATR0_INPUT_H__ */
