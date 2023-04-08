#pragma once
#ifndef __RATR0_EVENTS_H__
#define __RATR0_EVENTS_H__

/* Events subsystem */
struct Ratr0EventSystem {
    void (*shutdown)(void);
};

/**
 * Start up the events subsystem.
 */
extern struct Ratr0EventSystem *ratr0_events_startup(void);

#endif /* __RATR0_EVENTS_H__ */
