#pragma once
#ifndef __RATR0_INPUT_H__
#define __RATR0_INPUT_H__
#include <ratr0/engine.h>

/* Input subsystem */
struct Ratr0InputSystem {
    void (*shutdown)(void);
};

/**
 * Start up the input subsystem.
 */
extern struct Ratr0InputSystem *ratr0_input_startup(Ratr0Engine *);


#endif /* __RATR0_INPUT_H__ */
