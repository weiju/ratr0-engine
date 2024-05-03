#include <stdio.h>
#include <string.h>
#include <ratr0/fixed_point.h>
#include "../../chibi_test/chibi.h"

/*
 * TEST CASES
 */
int abs(int a) {
    return a < 0 ? -a : a;
}

#define EPS (2)
#define EPS2 (3)

/*
 * Check timer state after valid initialization.
 */
CHIBI_TEST(TestCreateAbsFixed24_8)
{
    FIXED8 f1 = FIXED8_CREATE_ABS(3, 14); // Simple PI
    chibi_assert_eq_int(3, FIXED8_INT_ABS(f1));
    chibi_assert(abs(14 - FIXED8_FRAC_ABS(f1)) < EPS);

    FIXED8 f2 = FIXED8_CREATE_ABS(13, 99); // Test the range at 99
    chibi_assert_eq_int(13, FIXED8_INT_ABS(f2));
    chibi_assert(abs(99 - FIXED8_FRAC(f2)) < EPS);
}

CHIBI_TEST(TestCreateNegFixed24_8)
{
    FIXED8 f1 = FIXED8_CREATE(-3, 14); // Simple PI
    chibi_assert_eq_int(-3, FIXED8_INT(f1));
    chibi_assert(abs(14 - FIXED8_FRAC(f1)) < EPS);

    FIXED8 f2 = FIXED8_CREATE(-13, 99); // Test the range at 99
    chibi_assert_eq_int(-13, FIXED8_INT(f2));
    chibi_assert(abs(99 - FIXED8_FRAC(f2)) < EPS);
}

CHIBI_TEST(TestAddFixed24_8)
{
    FIXED8 f1 = FIXED8_CREATE(2, 14);
    FIXED8 f2 = FIXED8_CREATE(3, 2);
    FIXED8 f3 = f1 + f2;
    chibi_assert_eq_int(5, FIXED8_INT(f3));
    chibi_assert(abs(16 - FIXED8_FRAC(f3)) < EPS);
}

CHIBI_TEST(TestAddFixed24_8WithCarry)
{
    FIXED8 f1 = FIXED8_CREATE(2, 85);
    FIXED8 f2 = FIXED8_CREATE(3, 20);
    FIXED8 f3 = f1 + f2;
    chibi_assert_eq_int(6, FIXED8_INT(f3));
    chibi_assert(abs(5 - FIXED8_FRAC(f3)) < EPS);
}


CHIBI_TEST(TestSubFixed24_8)
{
    FIXED8 f1 = FIXED8_CREATE(2, 14);
    FIXED8 f2 = FIXED8_CREATE(1, 2);
    FIXED8 f3 = f1 - f2;
    chibi_assert_eq_int(1, FIXED8_INT(f3));
    chibi_assert(abs(12 - FIXED8_FRAC(f3)) < EPS);
}

CHIBI_TEST(TestSubFixed24_8WithCarry)
{
    FIXED8 f1 = FIXED8_CREATE(2, 14);
    FIXED8 f2 = FIXED8_CREATE(0, 20);
    FIXED8 f3 = f1 - f2;
    chibi_assert_eq_int(1, FIXED8_INT(f3));
    chibi_assert(abs(94 - FIXED8_FRAC(f3)) < EPS);
}

CHIBI_TEST(TestMulFixed24_8)
{
    FIXED8 f1 = FIXED8_CREATE(2, 14);
    FIXED8 f2 = FIXED8_CREATE(3, 20);
    FIXED8 f3 = FIXED8_MUL(f1, f2);
    //printf("f3 = %d.%02d\n", FIXED_INT(f3), FIXED_FRAC(f3));
    chibi_assert_eq_int(6, FIXED8_INT(f3));
    chibi_assert(abs(85 - FIXED8_FRAC(f3)) < EPS2);
}

/**
 * 16:16 fixed point tests
 */

CHIBI_TEST(TestCreateAbsFixed16_16)
{
    FIXED16 f1 = FIXED16_CREATE_ABS(3, 14); // Simple PI
    chibi_assert_eq_int(3, FIXED16_INT_ABS(f1));
    chibi_assert(abs(14 - FIXED16_FRAC_ABS(f1)) < EPS);

    FIXED16 f2 = FIXED16_CREATE_ABS(13, 99); // Test the range at 99
    chibi_assert_eq_int(13, FIXED16_INT_ABS(f2));
    chibi_assert(abs(99 - FIXED16_FRAC(f2)) < EPS);
}

CHIBI_TEST(TestCreateNegFixed16_16)
{
    FIXED16 f1 = FIXED16_CREATE(-3, 14); // Simple PI
    chibi_assert_eq_int(-3, FIXED16_INT(f1));
    chibi_assert(abs(14 - FIXED16_FRAC(f1)) < EPS);

    FIXED16 f2 = FIXED16_CREATE(-13, 99); // Test the range at 99
    chibi_assert_eq_int(-13, FIXED16_INT(f2));
    chibi_assert(abs(99 - FIXED16_FRAC(f2)) < EPS);
}

CHIBI_TEST(TestAddFixed16_16)
{
    FIXED16 f1 = FIXED16_CREATE(2, 14);
    FIXED16 f2 = FIXED16_CREATE(3, 2);
    FIXED16 f3 = f1 + f2;
    chibi_assert_eq_int(5, FIXED16_INT(f3));
    chibi_assert(abs(16 - FIXED16_FRAC(f3)) < EPS);
}

CHIBI_TEST(TestAddFixed16_16WithCarry)
{
    FIXED16 f1 = FIXED16_CREATE(2, 980);
    FIXED16 f2 = FIXED16_CREATE(3, 50);
    FIXED16 f3 = f1 + f2;
    chibi_assert_eq_int(6, FIXED16_INT(f3));
    chibi_assert(abs(30 - FIXED16_FRAC(f3)) < EPS);
}

/*
 * SUITE DEFINITION
 */

chibi_suite *CoreSuite(void)
{
    chibi_suite *suite = chibi_suite_new_fixture("ratr0.FixedPointSuite", NULL, NULL, NULL);
    chibi_suite_add_test(suite, TestCreateAbsFixed24_8);
    chibi_suite_add_test(suite, TestCreateNegFixed24_8);
    chibi_suite_add_test(suite, TestAddFixed24_8);
    chibi_suite_add_test(suite, TestAddFixed24_8WithCarry);
    chibi_suite_add_test(suite, TestSubFixed24_8);
    chibi_suite_add_test(suite, TestSubFixed24_8WithCarry);
    chibi_suite_add_test(suite, TestMulFixed24_8);

    chibi_suite_add_test(suite, TestCreateAbsFixed16_16);
    chibi_suite_add_test(suite, TestCreateNegFixed16_16);
    chibi_suite_add_test(suite, TestAddFixed16_16);
    chibi_suite_add_test(suite, TestAddFixed16_16WithCarry);

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
