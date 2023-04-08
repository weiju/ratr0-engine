#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/timers.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[35mTIMERS\033[0m", __VA_ARGS__)

void ratr0_timers_shutdown(void);
static struct Ratr0TimerSystem timer_system;
static Ratr0Engine *engine;

/* Timer pool.
 * For now, this is just a small fixed-size pool of timers. We can't update that many
 * timers per frame anyway.
 */
#define MAX_TIMERS (20)
static Ratr0Timer timers[MAX_TIMERS];
static int next_free_timer = 0;

void ratr0_timers_update(Ratr0Timer *timer)
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

Ratr0Timer *ratr0_timers_create(INT32 start_value, BOOL oneshot, void (*timeout_fun)(void))
{
    Ratr0Timer *timer = &timers[next_free_timer++];
    timer->start_value = start_value;
    timer->current_value = start_value;
    timer->oneshot = oneshot;
    timer->running = 1;
    timer->timeout_fun = timeout_fun;
    return timer;
}

void ratr0_timers_tick(void)
{
}


struct Ratr0TimerSystem *ratr0_timers_startup(Ratr0Engine *eng)
{
    engine = eng;
    timer_system.shutdown = &ratr0_timers_shutdown;
    timer_system.create_timer = &ratr0_timers_create;
    timer_system.update = &ratr0_timers_tick;

    PRINT_DEBUG("Startup finished.");
    return &timer_system;
}

void ratr0_timers_shutdown(void)
{
    PRINT_DEBUG("Shutdown finished.");
}
