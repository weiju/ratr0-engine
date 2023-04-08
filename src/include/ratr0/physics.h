#pragma once
#ifndef __RATR0_PHYSICS_H__
#define __RATR0_PHYSICS_H__
#include <ratr0/engine.h>

/* Physics and collisions subsystem */
struct Ratr0PhysicsSystem {
    void (*shutdown)(void);
};

/**
 * Start up the physics subsystem.
 */
extern struct Ratr0PhysicsSystem *ratr0_physics_startup(Ratr0Engine *);


#endif /* __RATR0_PHYSICS_H__ */
