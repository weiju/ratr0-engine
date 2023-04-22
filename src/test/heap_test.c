#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ratr0/heap.h>
#include "../../chibi_test/chibi.h"

void heaptest_setup(void *userdata) { }
void heaptest_teardown(void *userdata) { }

typedef struct _coord { int x, y; } Coord;

int coord_greater(Coord *c1, Coord *c2) {
    return c1->y > c2->y || (c1->y == c2->y && c1->x > c2->y);
}

int coord_less(Coord *c1, Coord *c2) {
    return c1->y < c2->y || (c1->y == c2->y && c1->x < c2->y);
}

/*
 * TEST CASES
 */
CHIBI_TEST(TestSimpleHeap)
{
    Coord pool[5] = {
        { 3,  1}, { 5,  1}, { 1,  2}, { 7,  3}, { 11,  2}
    };
    Coord *array[10];
    int heap_size = 0;
    for (int i = 0; i < 5; i++) {
        heap_size = ratr0_heap_insert((void **) array, heap_size, &pool[i],
                                      (int (*)(void *, void *)) &coord_less);
    }
    Coord *max0 = ratr0_heap_extract_max((void **) array, &heap_size,
                                         (int (*)(void *, void *)) &coord_greater);
    chibi_assert(max0->x == 7 && max0->y == 3);
    chibi_assert_eq_int(4, heap_size);

    Coord *max1 = ratr0_heap_extract_max((void **) array, &heap_size,
                                         (int (*)(void *, void *)) &coord_greater);
    chibi_assert(max1->x == 11 && max1->y == 2);
    chibi_assert_eq_int(3, heap_size);

    Coord *max2 = ratr0_heap_extract_max((void **) array, &heap_size,
                                         (int (*)(void *, void *)) &coord_greater);
    chibi_assert(max2->x == 1 && max2->y == 2);
    chibi_assert_eq_int(2, heap_size);

    Coord *max3 = ratr0_heap_extract_max((void **) array, &heap_size,
                                         (int (*)(void *, void *)) &coord_greater);
    chibi_assert(max3->x == 5 && max3->y == 1);
    chibi_assert_eq_int(1, heap_size);

    Coord *max4 = ratr0_heap_extract_max((void **) array, &heap_size,
                                         (int (*)(void *, void *)) &coord_greater);
    chibi_assert(max4->x == 3 && max4->y == 1);
    chibi_assert_eq_int(0, heap_size);

    // Extract max on empty heap results in NULL being returned
    // signaling a heap underflow
    Coord *max5 = ratr0_heap_extract_max((void **) array, &heap_size,
                                         (int (*)(void *, void *)) &coord_greater);
    chibi_assert(max5 == NULL);
}

CHIBI_TEST(TestExtractMaxEmptyHeap)
{
    Coord *array[1];
    int heap_size = 0;
    Coord *max0 = ratr0_heap_extract_max((void **) array, &heap_size,
                                         (int (*)(void *, void *)) &coord_greater);
    chibi_assert(max0 == NULL);
    chibi_assert_eq_int(0, heap_size);
}
/*
 * SUITE DEFINITION
 */

chibi_suite *CoreSuite(void)
{
    chibi_suite *suite = chibi_suite_new_fixture("ratr0.HeapSuite", heaptest_setup,
                                                 heaptest_teardown, NULL);
    chibi_suite_add_test(suite, TestSimpleHeap);
    chibi_suite_add_test(suite, TestExtractMaxEmptyHeap);

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
