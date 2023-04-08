#include <stdio.h>
#include <string.h>
#include <ratr0/timers.h>
#include "../../chibi_test/chibi.h"

/*
 * TEST CASES
 */
void dummy_notify() { }

/*
 * Check timer state after valid initialization.
 */
CHIBI_TEST(TestInitTimer)
{
    struct Ratr0TimerSystem *timer_sys = ratr0_timers_startup(NULL);
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
    struct Ratr0TimerSystem *timer_sys = ratr0_timers_startup(NULL);
    Ratr0Timer *timer = timer_sys->create_timer(5, FALSE, dummy_notify);

    timer_sys->update();
    chibi_assert_eq_int(4, timer->current_value);
}

/*
 * Check that timer is not updated when timer is not running.
 */
CHIBI_TEST(TestUpdateTimerNotRunning)
{
    struct Ratr0TimerSystem *timer_sys = ratr0_timers_startup(NULL);
    Ratr0Timer *timer = timer_sys->create_timer(5, FALSE, dummy_notify);
    timer->running = FALSE;

    timer_sys->update();
    chibi_assert_eq_int(5, timer->current_value);
}

/*
 * Check that timer function is notified and value is reset when timer value reaches 0.
 */
static int was_notified1 = 0;
void notify1()
{
    was_notified1++;
}

CHIBI_TEST(TestUpdateTimerTimeout)
{
    struct Ratr0TimerSystem *timer_sys = ratr0_timers_startup(NULL);
    Ratr0Timer *timer = timer_sys->create_timer(3, 0, notify1);

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
void notify2()
{
    was_notified2++;
}

CHIBI_TEST(TestUpdateTimerTimeoutOneShot)
{
    struct Ratr0TimerSystem *timer_sys = ratr0_timers_startup(NULL);
    Ratr0Timer *timer = timer_sys->create_timer(3, TRUE, notify2);

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
 * SUITE DEFINITION
 */

chibi_suite *CoreSuite(void)
{
    chibi_suite *suite = chibi_suite_new_fixture("ratr0.TimerSuite", NULL, NULL, NULL);
    chibi_suite_add_test(suite, TestInitTimer);
    chibi_suite_add_test(suite, TestUpdateTimerIsRunning);
    chibi_suite_add_test(suite, TestUpdateTimerNotRunning);
    chibi_suite_add_test(suite, TestUpdateTimerTimeout);
    chibi_suite_add_test(suite, TestUpdateTimerTimeoutOneShot);

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
