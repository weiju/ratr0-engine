#pragma once
#ifndef __RATR0_PHYSICS_H__
#define __RATR0_PHYSICS_H__

/* Physics and collisions subsystem */
struct Ratr0PhysicsSystem {
    void (*shutdown)(void);
};

/**
 * Start up the physics subsystem.
 */
extern struct Ratr0PhysicsSystem *ratr0_physics_startup(void);


#endif /* __RATR0_PHYSICS_H__ */
