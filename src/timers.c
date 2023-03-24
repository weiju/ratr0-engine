#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/timers.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[35mTIMERS\033[0m", __VA_ARGS__)

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

void ratr0_init_timer(Ratr0Timer *timer, INT32 start_value, BOOL oneshot, void (*timeout_fun)(void))
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
    PRINT_DEBUG("Start up...");
    // TODO: Initialize a pool of timers
    PRINT_DEBUG("Startup finished.");
}

void ratr0_timers_shutdown()
{
    PRINT_DEBUG("Shutting down...");
    // TODO: Free resources
    PRINT_DEBUG("Shutdown finished.");
}
