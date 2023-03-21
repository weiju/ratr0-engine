#include <stdio.h>
#include <string.h>
#include <chibi.h>
#include <ratr0/timers.h>

/*
 * TEST CASES
 */
void dummy_notify() { }

CHIBI_TEST(TestInitTimer)
{
    Timer timer;
    init_timer(&timer, 5, 0, dummy_notify);
    chibi_assert_eq_int(5, timer.current_value);
    chibi_assert_eq_int(5, timer.start_value);
    chibi_assert_eq_int(0, timer.oneshot);
    chibi_assert_eq_int(1, timer.running);
}




/*
 * SUITE DEFINITION
 */

chibi_suite *CoreSuite(void)
{
    chibi_suite *suite = chibi_suite_new_fixture("ratr0.TimerSuite", NULL, NULL, NULL);
    chibi_suite_add_test(suite, TestInitTimer);

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
