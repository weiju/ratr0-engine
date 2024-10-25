#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ratr0/memory.h>
#include <ratr0/datastructs/treeset.h>
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

struct Ratr0TreeSets *tree_sets;

void treeset_test_setup(void *userdata)
{
    mock_mem = NULL;
    /*
    memsys.allocate_block = &mock_allocate_block;
    memsys.free_block = &mock_free_block;
    memsys.block_address = &mock_block_address;
    mock_engine.memory_system = &memsys;
    */
    tree_sets = ratr0_init_tree_sets(&mock_engine);
}
void treeset_test_teardown(void *userdata) {
    if (mock_mem) {
        free(mock_mem);
        mock_mem = NULL;
    }
    tree_sets->shutdown();
}

/*
 * TEST CASES
 */
struct Coord { int x, y; };
BOOL lt(void *a, void *b)
{
    struct Coord *aval = (struct Coord *) a;
    struct Coord *bval = (struct Coord *) b;

    return aval->y < bval->y || aval->y == bval->y && aval->x < bval->y;
}

BOOL eq(void *a, void *b) { return a == b; }

CHIBI_TEST(TestInitTreeSet)
{
    struct Ratr0TreeSet *tree_set = tree_sets->get_tree_set();
    chibi_assert(tree_set->root->color == RBT_BLACK);
    chibi_assert_eq_int(0, tree_set->num_elements);
}

CHIBI_TEST(TestInsertSingle)
{
    struct Ratr0TreeSet *tree_set = tree_sets->get_tree_set();
    struct Coord a = {1,2};
    ratr0_tree_set_insert(tree_set, &a, lt, eq);
    chibi_assert_not_null(tree_set->root);
    chibi_assert(&a == tree_set->root->value);
    chibi_assert_eq_int(1, tree_set->num_elements);
}

CHIBI_TEST(TestInsertTwoElements)
{
    struct Ratr0TreeSet *tree_set = tree_sets->get_tree_set();
    struct Coord a = {1,2};
    struct Coord b = {2,5};

    ratr0_tree_set_insert(tree_set, &a, lt, eq);
    ratr0_tree_set_insert(tree_set, &b, lt, eq);
    chibi_assert_not_null(tree_set->root);
    chibi_assert_eq_int(2, tree_set->num_elements);
}

CHIBI_TEST(TestInsertTwoElementsAndDuplicate)
{
    struct Ratr0TreeSet *tree_set = tree_sets->get_tree_set();
    struct Coord a = {1,2};
    struct Coord b = {2,5};

    ratr0_tree_set_insert(tree_set, &a, lt, eq);
    ratr0_tree_set_insert(tree_set, &b, lt, eq);
    ratr0_tree_set_insert(tree_set, &b, lt, eq);
    chibi_assert_not_null(tree_set->root);
    chibi_assert_eq_int(2, tree_set->num_elements);
}

CHIBI_TEST(TestInsertTwoElementsDuplicateAndOneMore)
{
    struct Ratr0TreeSet *tree_set = tree_sets->get_tree_set();
    struct Coord a = {1,2};
    struct Coord b = {2,5};
    struct Coord c = {3,5};

    ratr0_tree_set_insert(tree_set, &a, lt, eq);
    ratr0_tree_set_insert(tree_set, &b, lt, eq);
    ratr0_tree_set_insert(tree_set, &b, lt, eq);
    ratr0_tree_set_insert(tree_set, &c, lt, eq);
    chibi_assert_not_null(tree_set->root);
    chibi_assert_eq_int(3, tree_set->num_elements);
}

CHIBI_TEST(TestClearSet)
{
    struct Ratr0TreeSet *tree_set = tree_sets->get_tree_set();
    struct Coord a = {1,2};
    struct Coord b = {2,5};

    ratr0_tree_set_insert(tree_set, &a, lt, eq);
    ratr0_tree_set_insert(tree_set, &b, lt, eq);
    chibi_assert_eq_int(2, tree_set->num_elements);

    ratr0_tree_set_clear(tree_set);
    chibi_assert_eq_int(0, tree_set->num_elements);
}

/**
 * Iteration test. Simply count and sum the components.
 */
static int counter0 = 0, x0 = 0, y0 = 0;
static void process_node(struct Ratr0TreeSetNode *node, void *user_data)
{
    counter0++;
    x0 += ((struct Coord *) node->value)->x;
    y0 += ((struct Coord *) node->value)->y;
}

CHIBI_TEST(TestIterate)
{
    struct Ratr0TreeSet *tree_set = tree_sets->get_tree_set();
    struct Coord a = {1,2};
    struct Coord b = {2,5};

    ratr0_tree_set_insert(tree_set, &a, lt, eq);
    ratr0_tree_set_insert(tree_set, &b, lt, eq);

    ratr0_tree_set_iterate(tree_set, &process_node, NULL);
    chibi_assert_eq_int(2, counter0);
    chibi_assert_eq_int(3, x0);
    chibi_assert_eq_int(7, y0);
}

/*
 * SUITE DEFINITION
 */

chibi_suite *CoreSuite(void)
{
    chibi_suite *suite = chibi_suite_new_fixture("ratr0.TreeSetSuite",
                                                 treeset_test_setup,
                                                 treeset_test_teardown, NULL);
    chibi_suite_add_test(suite, TestInitTreeSet);
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

    chibi_suite_run(suite, &summary);
    chibi_suite_delete(suite);
    return summary.num_failures;
}
