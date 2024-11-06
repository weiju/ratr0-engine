#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ratr0/memory.h>
#include <ratr0/timers.h>
#include "../../chibi_test/chibi.h"

static Ratr0Engine mock_engine;
static struct Ratr0MemorySystem memsys;

static void *mock_mem;
Ratr0MemHandle ratr0_memory_allocate_block(Ratr0MemoryType mem_type, UINT32 size)
{
    mock_mem = malloc(size);
    return 0;
}
void ratr0_memory_free_block(Ratr0MemHandle handle) {
    if (mock_mem) {
        free(mock_mem);
        mock_mem = NULL;
    }
}
void *ratr0_memory_block_address(Ratr0MemHandle handle) { return mock_mem; }

void timertest_setup(void *userdata)
{
    mock_mem = NULL;
}

void timertest_teardown(void *userdata) {
    if (mock_mem) {
        free(mock_mem);
        mock_mem = NULL;
    }
}

/*
 * TEST CASES
 */
void dummy_notify() { }

/*
 * Check timer state after valid initialization.
 */
CHIBI_TEST(TestInitTimer)
{
    struct Ratr0TimerSystem *timer_sys = ratr0_timers_startup(&mock_engine, 5);
    Ratr0TimerHandle timer_handle = ratr0_timers_create(5, FALSE, dummy_notify);
    struct Ratr0Timer *timer = ratr0_timers_get(timer_handle);

    chibi_assert_eq_int(5, timer->current_value);
    chibi_assert_eq_int(5, timer->start_value);
    chibi_assert_eq_int(FALSE, timer->oneshot);
    chibi_assert_eq_int(TRUE, timer->running);
    chibi_assert(dummy_notify == timer->timeout_fun);
}

/*
 * Check timer is counting down if running.
 */
CHIBI_TEST(TestUpdateTimerIsRunning)
{
    struct Ratr0TimerSystem *timer_sys = ratr0_timers_startup(&mock_engine, 5);
    Ratr0TimerHandle timer_handle = ratr0_timers_create(5, FALSE, dummy_notify);
    struct Ratr0Timer *timer = ratr0_timers_get(timer_handle);

    ratr0_timers_tick();
    chibi_assert_eq_int(4, timer->current_value);
}

/*
 * Check that timer is not updated when timer is not running.
 */
CHIBI_TEST(TestUpdateTimerNotRunning)
{
    struct Ratr0TimerSystem *timer_sys = ratr0_timers_startup(&mock_engine, 5);
    Ratr0TimerHandle timer_handle = ratr0_timers_create(5, FALSE, dummy_notify);
    struct Ratr0Timer *timer = ratr0_timers_get(timer_handle);
    timer->running = FALSE;

    ratr0_timers_tick();
    chibi_assert_eq_int(5, timer->current_value);
}

/*
 * Check that timer function is notified and value is reset when timer value reaches 0.
 */
static int was_notified1 = 0;
void notify01()
{
    was_notified1++;
}

CHIBI_TEST(TestUpdateTimerTimeout)
{
    struct Ratr0TimerSystem *timer_sys = ratr0_timers_startup(&mock_engine, 5);
    Ratr0TimerHandle timer_handle = ratr0_timers_create(3, 0, notify01);
    struct Ratr0Timer *timer = ratr0_timers_get(timer_handle);

    /* Count down the timer */
    for (int i = 0; i < 3; i++) {
        ratr0_timers_tick();
    }

    chibi_assert_eq_int(1, was_notified1);
    chibi_assert_eq_int(3, timer->current_value);

    /* Count down the timer a second time */
    for (int i = 0; i < 3; i++) {
        ratr0_timers_tick();
    }

    /* now there are 2 notifications */
    chibi_assert_eq_int(2, was_notified1);
    chibi_assert_eq_int(3, timer->current_value);
}

/*
 * Check that timer function is notified and value is reset when timer value reaches 0.
 * This is one-shot mode, so the timer will be stopped and the counter is 0 at the end
 */
static int was_notified2 = 0;
void notify02()
{
    was_notified2++;
}

CHIBI_TEST(TestUpdateTimerTimeoutOneShot)
{
    struct Ratr0TimerSystem *timer_sys = ratr0_timers_startup(&mock_engine, 5);
    Ratr0TimerHandle timer_handle = ratr0_timers_create(3, TRUE, notify02);
    struct Ratr0Timer *timer = ratr0_timers_get(timer_handle);

    /* Count down the timer */
    for (int i = 0; i < 3; i++) {
        ratr0_timers_tick();
    }

    chibi_assert_eq_int(1, was_notified2);
    chibi_assert_eq_int(0, timer->current_value);
    chibi_assert_eq_int(FALSE, timer->running);

    /* Count down the timer a second time */
    for (int i = 0; i < 3; i++) {
        ratr0_timers_tick();
    }

    /* still only 1 notification */
    chibi_assert_eq_int(1, was_notified2);
    chibi_assert_eq_int(0, timer->current_value);
}

/*
 * Check that timer function is notified and value is reset when timer value reaches 0.
 */
static int was_notified03 = 0;
static int was_notified04 = 0;
void notify03() { was_notified03++; }
void notify04() { was_notified04++; }


CHIBI_TEST(TestUpdate2TimersTimeout)
{
    struct Ratr0TimerSystem *timer_sys = ratr0_timers_startup(&mock_engine, 5);
    Ratr0TimerHandle timer_handle1 = ratr0_timers_create(3, 0, notify03);
    Ratr0TimerHandle timer_handle2 = ratr0_timers_create(3, 0, notify04);
    struct Ratr0Timer *timer1 = ratr0_timers_get(timer_handle1);
    struct Ratr0Timer *timer2 = ratr0_timers_get(timer_handle2);

    /* Count down the timer */
    for (int i = 0; i < 3; i++) {
        ratr0_timers_tick();
    }
    /* 2 notifications were sent */
    chibi_assert_eq_int(1, was_notified03);
    chibi_assert_eq_int(1, was_notified04);
}

CHIBI_TEST(TestCreateTooManyTimers)
{
    struct Ratr0TimerSystem *timer_sys = ratr0_timers_startup(&mock_engine, 2);
    Ratr0TimerHandle timer_handle1 = ratr0_timers_create(3, 0, NULL);
    Ratr0TimerHandle timer_handle2 = ratr0_timers_create(3, 0, NULL);
    Ratr0TimerHandle timer_handle3 = ratr0_timers_create(3, 0, NULL);

    chibi_assert_eq_int(-1, timer_handle3);
}

CHIBI_TEST(TestCreateAndFreeTimers)
{
    struct Ratr0TimerSystem *timer_sys = ratr0_timers_startup(&mock_engine, 2);
    Ratr0TimerHandle timer_handle1 = ratr0_timers_create(3, 0, NULL);
    Ratr0TimerHandle timer_handle2 = ratr0_timers_create(3, 0, NULL);

    ratr0_timers_free(timer_handle1);
    Ratr0TimerHandle timer_handle3 = ratr0_timers_create(3, 0, NULL);

    // this is the handle of the freed object
    chibi_assert_eq_int(timer_handle1, timer_handle3);

    struct Ratr0Timer *timer2 = ratr0_timers_get(timer_handle2);
    chibi_assert_eq_int(-1, timer2->next);
}


/*
 * SUITE DEFINITION
 */

chibi_suite *CoreSuite(void)
{
    chibi_suite *suite = chibi_suite_new_fixture("ratr0.TimerSuite", timertest_setup,
                                                 timertest_teardown, NULL);

    chibi_suite_add_test(suite, TestInitTimer);
    chibi_suite_add_test(suite, TestUpdateTimerIsRunning);
    chibi_suite_add_test(suite, TestUpdateTimerNotRunning);
    chibi_suite_add_test(suite, TestUpdateTimerTimeout);
    chibi_suite_add_test(suite, TestUpdateTimerTimeoutOneShot);
    chibi_suite_add_test(suite, TestUpdate2TimersTimeout);
    chibi_suite_add_test(suite, TestCreateTooManyTimers);
    chibi_suite_add_test(suite, TestCreateAndFreeTimers);
    return suite;
}

int main(int argc, char **argv)
{
    chibi_summary_data summary;
    chibi_suite *suite = CoreSuite();

    chibi_suite_run(suite, &summary);
    chibi_suite_delete(suite);
    return summary.num_failures;
}
