#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ratr0/datastructs/bitset.h>
#include "../../chibi_test/chibi.h"

void bitset_test_setup(void *userdata) { }
void bitset_test_teardown(void *userdata) { }

/*
 * TEST CASES
 */
CHIBI_TEST(TestClearBitSet)
{
    UINT32 bitset_arr[4] = {1,2,3,4};
    ratr0_bitset_clear(bitset_arr, 4);
    for (int i = 0; i < 4; i++) {
        chibi_assert_eq_int(0, bitset_arr[i]);
    }
}

CHIBI_TEST(TestInsertIsSet)

{
    UINT32 bitset_arr[4];
    ratr0_bitset_clear(bitset_arr, 4);

    // Element 0, the simplest one
    ratr0_bitset_insert(bitset_arr, 4, 0);
    chibi_assert((bitset_arr[0] & 0x80000000) == 0x80000000);
    chibi_assert(ratr0_bitset_isset(bitset_arr, 4, 0));

    // Element 33, this should be harder
    ratr0_bitset_insert(bitset_arr, 4, 33);
    chibi_assert((bitset_arr[1] & 0x40000000) == 0x40000000);
    chibi_assert(ratr0_bitset_isset(bitset_arr, 4, 33));
}

void process_bit(UINT16 index)
{
    printf("BIT SET INDEX: %d\n", index);
}

CHIBI_TEST(TestIterate)

{
    UINT32 bitset_arr[4];
    ratr0_bitset_clear(bitset_arr, 4);

    ratr0_bitset_insert(bitset_arr, 4, 0);
    ratr0_bitset_insert(bitset_arr, 4, 33);
    ratr0_bitset_insert(bitset_arr, 4, 67);
    ratr0_bitset_iterate(bitset_arr, 4, &process_bit);
}

/*
 * SUITE DEFINITION
 */

chibi_suite *CoreSuite(void)
{
    chibi_suite *suite = chibi_suite_new_fixture("ratr0.BitSetSuite", bitset_test_setup,
                                                 bitset_test_teardown, NULL);
    chibi_suite_add_test(suite, TestClearBitSet);
    chibi_suite_add_test(suite, TestInsertIsSet);
    chibi_suite_add_test(suite, TestIterate);

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
