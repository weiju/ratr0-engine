/** @file events.h
 *
 * Events subsystem
 */
#pragma once
#ifndef __RATR0_EVENTS_H__
#define __RATR0_EVENTS_H__
#include <ratr0/engine.h>

/**
 * Interface to events subsystem.
 */
struct Ratr0EventSystem {
    /**
     * Shuts down the event subsystem.
     */
    void (*shutdown)(void);
};

/**
 * Start up the events subsystem.
 *
 * @param engine pointer to Ratr0Engine instance
 * @return pointer to initialized event system
 */
extern struct Ratr0EventSystem *ratr0_events_startup(Ratr0Engine *engine);

#endif /* __RATR0_EVENTS_H__ */
