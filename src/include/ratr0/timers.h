/** @file timers.h
 *
 * RATR0 Timer Subsystem. This system helps with the management of timers.
 */
#pragma once
#ifndef __RATR0_TIMERS_H__
#define __RATR0_TIMERS_H__
#include <ratr0/engine.h>

#include <ratr0/data_types.h>

/**
 * Data structure for a timer object. A timer counts from the start value
 * down to 0 and then either restarts or stops.
 */
typedef struct _Timer {
    /** \brief timer start value */
    INT32 start_value;
    /** \brief timer current value */
    INT32 current_value;
    /** \brief if set to TRUE, timer stops after reaching 0 */
    BOOL oneshot;
    /** \brief timer status, if TRUE, the timer is running */
    BOOL running;

    /**
     * Timeout callback. Called when a timer reaches 0.
     */
    void (*timeout_fun)(void);

    // private, for management of free timers in the pool
    /** \brief next free timer in the pool */
    UINT16 next;
    /** \brief previous free timer in the pool */
    UINT16 prev;
} Ratr0Timer;

/**
 * Interface to the timer system.
 */
struct Ratr0TimerSystem {
    /**
     * Create a timer object with the specified behavior.
     *
     * @param start_value timer start value
     * @param oneshot TRUE if this is a one-shot timer, FALSE otherwise
     * @param timeout_fun user-provided timeout callback
     * @return pointer to initialize Ratr0Timer object
     */
    Ratr0Timer *(*create_timer)(INT32 start_value, BOOL oneshot, void (*timeout_fun)(void));

    /**
     * This function is called every frame to update the timers.
     */
    void (*update)(void);  // update all timers with the next interval step

    /**
     * Shuts down the timer system.
     */
    void (*shutdown)(void);
};

/**
 * Starts up the timer subsystem.
 *
 * @param engine the RATR0 Engine instance
 * @param pool_size the size of the timer pool
 * @return pointer to the initialized timer system
 */
extern struct Ratr0TimerSystem *ratr0_timers_startup(Ratr0Engine *engine, UINT16 pool_size);

#endif /* __RATR0_TIMERS_H__ */
