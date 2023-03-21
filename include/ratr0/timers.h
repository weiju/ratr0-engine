#pragma once
#ifndef __RATR0_TIMERS_H__
#define __RATR0_TIMERS_H__

/**
 * RATR0 Timer Subsystem. This system helps with the management of timers.
 */
#ifdef __VBCC__
#include <exec/types.h>
#else
#include <stdint.h>
typedef int32_t LONG;
typedef int16_t BOOL;
#endif


typedef struct _Timer {
    LONG start_value;
    LONG current_value;
    BOOL oneshot;
    BOOL running;
    void (*timeout_fun)(void);
} Ratr0Timer;

/**
 * Updates a timer instance.
 */
extern void ratr0_update_timer(Ratr0Timer *timer);

/**
 * Initializes a timer instance.
 */
extern void ratr0_init_timer(Ratr0Timer *timer, LONG start_value, BOOL oneshot,
                             void (*timeout_fun)(void));

/**
 * Start up the timer subsystem.
 */
extern void ratr0_timers_startup();

/**
 * Shut down the timer subsystem.
 */
extern void ratr0_timers_shutdown();

#endif /* __RATR0_TIMERS_H__ */
