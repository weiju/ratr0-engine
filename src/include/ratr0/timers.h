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
    INT16 next, prev; // private, for management
    void (*timeout_fun)(void);
} Ratr0Timer;

struct Ratr0TimerSystem {
    Ratr0Timer *(*create_timer)(INT32 start_value, BOOL oneshot, void (*timeout_fun)(void));
    void (*update)(void);  // update all timers with the next interval step
    void (*shutdown)(void);
};

/**
 * Start up the timer subsystem.
 */
extern struct Ratr0TimerSystem *ratr0_timers_startup(Ratr0Engine *);

#endif /* __RATR0_TIMERS_H__ */
