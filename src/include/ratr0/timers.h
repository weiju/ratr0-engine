/** @file timers.h
 *
 * RATR0 Timer Subsystem. This system helps with the management of timers.
 * Timers are updated by the Vertical blank interrupt, therefore a timer
 * tick is 1/60 of a second on an NTSC system while it is 1/50 of a second
 * on a PAL system.
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
struct Ratr0Timer {
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
    /** \brief next free timer in the pool, -1 is undefined  */
    INT16 next;
    /** \brief previous free timer in the pool, -1 is undefined */
    INT16 prev;
};

typedef UINT32 Ratr0TimerHandle;
/**
 * Interface to the timer system.
 */
struct Ratr0TimerSystem {

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

/**
 * Create a timer object with the specified behavior.
 *
 * @param start_value timer start value
 * @param oneshot TRUE if this is a one-shot timer, FALSE otherwise
 * @param timeout_fun user-provided timeout callback
 * @return handle to the Ratr0Timer object
 */
extern Ratr0TimerHandle ratr0_timers_create(INT32 start_value,
                                            BOOL oneshot,
                                            void (*timeout_fun)(void));


/**
 * Retrieve the Ratr0Timer object associated with the specified handle.
 *
 * @param handle handle to the Ratr0Timer object
 * @return pointer to the Ratr0Timer object
 */
extern struct Ratr0Timer *ratr0_timers_get(Ratr0TimerHandle handle);

/**
 * Free a timer object.
 *
 * @param handle handle to the Ratr0Timer object
 */
extern void ratr0_timers_free(Ratr0TimerHandle handle);

/**
 * This function is called every frame to update the timers.
 */
extern void ratr0_timers_tick(void);

#endif /* __RATR0_TIMERS_H__ */
