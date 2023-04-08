#pragma once
#ifndef __RATR0_EVENTS_H__
#define __RATR0_EVENTS_H__
#include <ratr0/engine.h>

/* Events subsystem */
struct Ratr0EventSystem {
    void (*shutdown)(void);
};

/**
 * Start up the events subsystem.
 */
extern struct Ratr0EventSystem *ratr0_events_startup(Ratr0Engine *);

#endif /* __RATR0_EVENTS_H__ */
