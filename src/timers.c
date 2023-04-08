#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/memory.h>
#include <ratr0/timers.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[35mTIMERS\033[0m", __VA_ARGS__)

void ratr0_timers_shutdown(void);
static struct Ratr0TimerSystem timer_system;
static Ratr0Engine *engine;

/* Timer pool.
 * We manage timers in a double linked list, so insertion and removal
 * are constant time operations.
 */
static Ratr0MemHandle h_timers;
static Ratr0Timer *timers;

/* Timer pool management variables */
static UINT16 max_timers;
static UINT16 num_used_timers = 0;
static INT16 first_free_timer = 0;

// We insert timers at the front, and iterate from the first
// timer, to keep things simple
static INT16 first_used_timer = -1;


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
    /* can't create more */
    if (num_used_timers == max_timers) {
        PRINT_DEBUG("maximum number of timers (%d) exceeded !", (int) max_timers);
        return NULL;
    }
    int next_free_timer = timers[first_free_timer].next;  // next in chain
    int timer_idx = first_free_timer;

    Ratr0Timer *timer = &timers[timer_idx];
    timer->start_value = start_value;
    timer->current_value = start_value;
    timer->oneshot = oneshot;
    timer->running = 1;
    timer->timeout_fun = timeout_fun;

    // put it in the chain
    if (first_used_timer == -1) {
        first_used_timer = timer_idx;
        timers[timer_idx].prev = timers[timer_idx].next = -1;
    } else {
        timers[first_used_timer].prev = timer_idx;
        timers[timer_idx].next = first_used_timer;
        timers[timer_idx].prev = -1; // make sure it's initialized correctly
        first_used_timer = timer_idx;
    }

    // This was a fresh timer, so just increment the free index
    if (next_free_timer == -1) first_free_timer++;
    num_used_timers++;
    return timer;
}

void ratr0_timers_tick(void)
{
    INT16 cur = first_used_timer;
    while (cur != -1) {
        ratr0_timers_update(&timers[cur]);
        cur = timers[cur].next;
    }
}

struct Ratr0TimerSystem *ratr0_timers_startup(Ratr0Engine *eng, UINT16 pool_size)
{
    engine = eng;
    timer_system.shutdown = &ratr0_timers_shutdown;
    timer_system.create_timer = &ratr0_timers_create;
    timer_system.update = &ratr0_timers_tick;

    // Initialize the timer pool
    max_timers = pool_size;
    h_timers = engine->memory_system->allocate_block(RATR0_MEM_DEFAULT,
                                                     sizeof(Ratr0Timer) * pool_size);
    timers = engine->memory_system->block_address(h_timers);
    for (int i = 0; i < pool_size; i++) {
        timers[i].running = FALSE;
        timers[i].next = timers[i].prev = -1;
    }
    num_used_timers = 0;
    first_free_timer = 0;
    first_used_timer = -1;

    PRINT_DEBUG("Startup finished.");
    return &timer_system;
}

void ratr0_timers_shutdown(void)
{
    engine->memory_system->free_block(h_timers);
    PRINT_DEBUG("Shutdown finished.");
}
