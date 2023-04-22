#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ratr0/memory.h>
#include <ratr0/coord_set.h>
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

struct CoordSets *coord_sets;

void coordtest_setup(void *userdata)
{
    mock_mem = NULL;
    memsys.allocate_block = &mock_allocate_block;
    memsys.free_block = &mock_free_block;
    memsys.block_address = &mock_block_address;
    mock_engine.memory_system = &memsys;
    coord_sets = ratr0_startup_coord_sets(&mock_engine);
}
void coordtest_teardown(void *userdata) {
    if (mock_mem) {
        free(mock_mem);
        mock_mem = NULL;
    }
    coord_sets->shutdown();
}

/*
 * TEST CASES
 */
CHIBI_TEST(TestInitCoordSet)
{
    struct CoordSet *coord_set = coord_sets->get_coord_set();
    chibi_assert(coord_set->root->color == RBT_BLACK);
    chibi_assert_eq_int(0, coord_set->num_elements);
}


/*
 * Insert single element
 */
CHIBI_TEST(TestInsertSingle)
{
    struct CoordSet *coord_set = coord_sets->get_coord_set();
    coord_set_insert(coord_set, 1, 2);
    chibi_assert_not_null(coord_set->root);
    chibi_assert_eq_int(1, coord_set->root->x);
    chibi_assert_eq_int(2, coord_set->root->y);
    chibi_assert_eq_int(1, coord_set->num_elements);
}

CHIBI_TEST(TestInsertTwoElements)
{
    struct CoordSet *coord_set = coord_sets->get_coord_set();
    coord_set_insert(coord_set, 1, 2);
    coord_set_insert(coord_set, 2, 5);
    chibi_assert_not_null(coord_set->root);
    chibi_assert_eq_int(2, coord_set->num_elements);
}

CHIBI_TEST(TestInsertTwoElementsAndDuplicate)
{
    struct CoordSet *coord_set = coord_sets->get_coord_set();
    coord_set_insert(coord_set, 1, 2);
    coord_set_insert(coord_set, 2, 5);
    coord_set_insert(coord_set, 2, 5);
    chibi_assert_not_null(coord_set->root);
    chibi_assert_eq_int(2, coord_set->num_elements);
}

CHIBI_TEST(TestInsertTwoElementsDuplicateAndOneMore)
{
    struct CoordSet *coord_set = coord_sets->get_coord_set();
    coord_set_insert(coord_set, 1, 2);
    coord_set_insert(coord_set, 2, 5);
    coord_set_insert(coord_set, 2, 5);
    coord_set_insert(coord_set, 3, 5);
    chibi_assert_not_null(coord_set->root);
    chibi_assert_eq_int(3, coord_set->num_elements);
}

CHIBI_TEST(TestClearSet)
{
    struct CoordSet *coord_set = coord_sets->get_coord_set();
    coord_set_insert(coord_set, 1, 2);
    coord_set_insert(coord_set, 2, 5);
    chibi_assert_eq_int(2, coord_set->num_elements);

    coord_set_clear(coord_set);
    chibi_assert_eq_int(0, coord_set->num_elements);
}

/**
 * Iteration test. Simply count and sum the components.
 */
static int counter0 = 0, x0 = 0, y0 = 0;
static void process_coord(struct Coord *node, void *user_data)
{
    counter0++;
    x0 += node->x;
    y0 += node->y;
}
CHIBI_TEST(TestIterate)
{
    struct CoordSet *coord_set = coord_sets->get_coord_set();
    coord_set_insert(coord_set, 1, 2);
    coord_set_insert(coord_set, 2, 5);

    coord_set_iterate(coord_set, &process_coord, NULL);
    chibi_assert_eq_int(2, counter0);
    chibi_assert_eq_int(3, x0);
    chibi_assert_eq_int(7, y0);
}

/*
 * SUITE DEFINITION
 */

chibi_suite *CoreSuite(void)
{
    chibi_suite *suite = chibi_suite_new_fixture("ratr0.CoordSetSuite", coordtest_setup,
                                                 coordtest_teardown, NULL);
    chibi_suite_add_test(suite, TestInitCoordSet);
    chibi_suite_add_test(suite, TestInsertSingle);
    chibi_suite_add_test(suite, TestInsertTwoElements);
    chibi_suite_add_test(suite, TestInsertTwoElementsAndDuplicate);
    chibi_suite_add_test(suite, TestInsertTwoElementsDuplicateAndOneMore);
    chibi_suite_add_test(suite, TestClearSet);
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
