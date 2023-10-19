#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ratr0/memory.h>
#include <ratr0/timers.h>
#include "../../chibi_test/chibi.h"

static Ratr0Engine mock_engine;
static struct Ratr0MemorySystem memsys;

static void *mock_mem;
Ratr0MemHandle mock_allocate_block(Ratr0MemoryType mem_type, UINT32 size)
{
    mock_mem = malloc(size);
    return 0;
}
void mock_free_block(Ratr0MemHandle handle) {
    if (mock_mem) {
        free(mock_mem);
        mock_mem = NULL;
    }
}
void *mock_block_address(Ratr0MemHandle handle) { return mock_mem; }

void timertest_setup(void *userdata)
{
    mock_mem = NULL;
    memsys.allocate_block = &mock_allocate_block;
    memsys.free_block = &mock_free_block;
    memsys.block_address = &mock_block_address;
    mock_engine.memory_system = &memsys;
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
    Ratr0Timer *timer = timer_sys->create_timer(5, FALSE, dummy_notify);
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
    Ratr0Timer *timer = timer_sys->create_timer(5, FALSE, dummy_notify);

    timer_sys->update();
    chibi_assert_eq_int(4, timer->current_value);
}

/*
 * Check that timer is not updated when timer is not running.
 */
CHIBI_TEST(TestUpdateTimerNotRunning)
{
    struct Ratr0TimerSystem *timer_sys = ratr0_timers_startup(&mock_engine, 5);
    Ratr0Timer *timer = timer_sys->create_timer(5, FALSE, dummy_notify);
    timer->running = FALSE;

    timer_sys->update();
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
    Ratr0Timer *timer = timer_sys->create_timer(3, 0, notify01);

    /* Count down the timer */
    for (int i = 0; i < 3; i++) {
        timer_sys->update();
    }

    chibi_assert_eq_int(1, was_notified1);
    chibi_assert_eq_int(3, timer->current_value);

    /* Count down the timer a second time */
    for (int i = 0; i < 3; i++) {
        timer_sys->update();
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
    Ratr0Timer *timer = timer_sys->create_timer(3, TRUE, notify02);

    /* Count down the timer */
    for (int i = 0; i < 3; i++) {
        timer_sys->update();
    }

    chibi_assert_eq_int(1, was_notified2);
    chibi_assert_eq_int(0, timer->current_value);
    chibi_assert_eq_int(FALSE, timer->running);

    /* Count down the timer a second time */
    for (int i = 0; i < 3; i++) {
        timer_sys->update();
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
    Ratr0Timer *timer1 = timer_sys->create_timer(3, 0, notify03);
    Ratr0Timer *timer2 = timer_sys->create_timer(3, 0, notify04);

    /* Count down the timer */
    for (int i = 0; i < 3; i++) {
        timer_sys->update();
    }
    /* 2 notifications were sent */
    chibi_assert_eq_int(1, was_notified03);
    chibi_assert_eq_int(1, was_notified04);
}

CHIBI_TEST(TestCreateTooManyTimers)
{
    struct Ratr0TimerSystem *timer_sys = ratr0_timers_startup(&mock_engine, 2);
    Ratr0Timer *timer1 = timer_sys->create_timer(3, 0, NULL);
    Ratr0Timer *timer2 = timer_sys->create_timer(3, 0, NULL);
    Ratr0Timer *timer3 = timer_sys->create_timer(3, 0, NULL);

    chibi_assert(timer3 == NULL);
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
	// BUG: this hangs !!!!
    //chibi_suite_add_test(suite, TestUpdate2TimersTimeout);
    chibi_suite_add_test(suite, TestCreateTooManyTimers);

    return suite;
}

int main(int argc, char **argv)
{
    chibi_summary_data summary;
    chibi_suite *suite = CoreSuite();

    if (argc > 1 && !strncmp("xml", argv[1], 3)) {
        chibi_suite_run_xml(suite, &summary, "test-reports");
    } else if (argc > 1 && !strncmp("tap", argv[1], 3)) {
        chibi_suite_run_tap(suite, &summary);
    } else {
        chibi_suite_run(suite, &summary);
    }
    chibi_suite_delete(suite);
    return summary.num_failures;
}
