#pragma once
#ifndef __RATR0_TIMERS_H__
#define __RATR0_TIMERS_H__
#include <ratr0/engine.h>

/**
 * RATR0 Timer Subsystem. This system helps with the management of timers.
 */
#include <ratr0/data_types.h>


typedef struct _Timer {
    INT32 start_value;
    INT32 current_value;
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
extern void ratr0_init_timer(Ratr0Timer *timer, INT32 start_value, BOOL oneshot,
                             void (*timeout_fun)(void));

struct Ratr0TimerSystem {
    void (*shutdown)(void);
};

/**
 * Start up the timer subsystem.
 */
extern struct Ratr0TimerSystem *ratr0_timers_startup(Ratr0Engine *);

#endif /* __RATR0_TIMERS_H__ */
