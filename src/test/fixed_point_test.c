#include <stdio.h>
#include <string.h>
#include <ratr0/fixed_point.h>
#include "../../chibi_test/chibi.h"

/*
 * TEST CASES
 */

/*
 * Check timer state after valid initialization.
 */
CHIBI_TEST(TestCreateFixed32)
{
    FIXED f = FIXED_CREATE_ABS(3, 1); // Simple PI
    chibi_assert_eq_int(3, FIXED_INT(f));
    chibi_assert_eq_int(1, FIXED_FRAC(f));
}

/*
 * SUITE DEFINITION
 */

chibi_suite *CoreSuite(void)
{
    chibi_suite *suite = chibi_suite_new_fixture("ratr0.FixedPointSuite", NULL, NULL, NULL);
    chibi_suite_add_test(suite, TestCreateFixed32);

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
