#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/timers.h>

void ratr0_update_timer(Ratr0Timer *timer)
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

void ratr0_init_timer(Ratr0Timer *timer, LONG start_value, BOOL oneshot, void (*timeout_fun)(void))
{
    if (timer) {
        timer->start_value = start_value;
        timer->current_value = start_value;
        timer->oneshot = oneshot;
        timer->running = 1;
        timer->timeout_fun = timeout_fun;
    }
}

void ratr0_timers_startup()
{
    PRINT_DEBUG("TIMERS - Start up...");
    // TODO: Initialize a pool of timers
    PRINT_DEBUG("Done.\n");
}

void ratr0_timers_shutdown()
{
    PRINT_DEBUG("TIMERS - Shutting down...");
    // TODO: Free resources
    PRINT_DEBUG("Done.\n");
}

