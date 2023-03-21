#pragma once
#ifndef __RATR0_TIMERS_H__
#define __RATR0_TIMERS_H__

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

extern void ratr0_update_timer(Ratr0Timer *timer);
extern void ratr0_init_timer(Ratr0Timer *timer, LONG start_value, BOOL oneshot,
                             void (*timeout_fun)(void));

#endif /* __RATR0_TIMERS_H__ */
