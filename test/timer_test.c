#include <stdio.h>
#include <string.h>
#include <chibi.h>
#include <ratr0/timers.h>

/*
 * TEST CASES
 */
void dummy_notify() { }

/*
 * Check timer state after valid initialization.
 */
CHIBI_TEST(TestInitTimer)
{
    Ratr0Timer timer;
    ratr0_init_timer(&timer, 5, 0, dummy_notify);
    chibi_assert_eq_int(5, timer.current_value);
    chibi_assert_eq_int(5, timer.start_value);
    chibi_assert_eq_int(0, timer.oneshot);
    chibi_assert_eq_int(1, timer.running);
    chibi_assert(dummy_notify == timer.timeout_fun);
}

/*
 * Check timer is counting down if running.
 */
CHIBI_TEST(TestUpdateTimerIsRunning)
{
    Ratr0Timer timer;
    ratr0_init_timer(&timer, 5, 0, dummy_notify);

    ratr0_update_timer(&timer);
    chibi_assert_eq_int(4, timer.current_value);
}

/*
 * Check that timer is not updated when timer is not running.
 */
CHIBI_TEST(TestUpdateTimerNotRunning)
{
    Ratr0Timer timer;
    ratr0_init_timer(&timer, 5, 0, dummy_notify);
    timer.running = 0;

    ratr0_update_timer(&timer);
    chibi_assert_eq_int(5, timer.current_value);
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
