#include "timers.h"
#include <stdio.h>

void update_timer(Timer *timer)
{
    if (timer && timer->running) {
        timer->current_value--;
        if (timer->current_value <= 0) {
            if (!timer->oneshot) {
                timer->current_value = timer->start_value;
            } else {
                timer->running = 0;
            }
            timer->timeout_fun();
        }
    }
}

void init_timer(Timer *timer, LONG start_value, BOOL oneshot, void (*timeout_fun)(void))
{
    if (timer) {
        timer->start_value = start_value;
        timer->current_value = start_value;
        timer->oneshot = oneshot;
        timer->running = 1;
        timer->timeout_fun = timeout_fun;
    }
}
