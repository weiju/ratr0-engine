#pragma once
#ifndef __TIMERS_H__
#define __TIMERS_H__

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
} Timer;

extern void update_timer(Timer *timer);
extern void init_timer(Timer *timer, LONG start_value, BOOL oneshot, void (*timeout_fun)(void));

#endif /* __TIMERS_H__ */
