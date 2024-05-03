#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ratr0/memory.h>
#include <ratr0/datastructs/quadtree.h>
#include <ratr0/datastructs/vector.h>
#include "../../chibi_test/chibi.h"

static Ratr0Engine mock_engine;
static struct Ratr0MemorySystem memsys;

static void *mock_mem[10];
int num_mem_entries;

Ratr0MemHandle mock_allocate_block(Ratr0MemoryType mem_type, UINT32 size)
{
    Ratr0MemHandle handle = num_mem_entries;
    mock_mem[num_mem_entries++] = malloc(size);
    return handle;
}
void mock_free_block(Ratr0MemHandle handle) {
    free(mock_mem[handle]);
    mock_mem[handle] = NULL;
}
void *mock_block_address(Ratr0MemHandle handle) { return mock_mem[handle]; }

void quadtreetest_setup(void *userdata)
{
    num_mem_entries = 0;
    memsys.allocate_block = &mock_allocate_block;
    memsys.free_block = &mock_free_block;
    memsys.block_address = &mock_block_address;
    mock_engine.memory_system = &memsys;
    ratr0_vector_startup(&mock_engine);
}

void quadtreetest_teardown(void *userdata) {
    for (int i = 0; i < num_mem_entries; i++) {
        if (mock_mem[i]) {
            free(mock_mem[i]);
            mock_mem[i] = NULL;
        }
    }
    num_mem_entries = 0;
}

/*
 * TEST CASES
 */
CHIBI_TEST(TestCreateQuadTree)
{
    struct Ratr0QuadTreeNode *node = ratr0_new_quad_tree(0, 0, 320, 256);
    chibi_assert_eq_int(0, node->bounds.x);
    chibi_assert_eq_int(0, node->bounds.y);
    chibi_assert_eq_int(320, node->bounds.width);
    chibi_assert_eq_int(256, node->bounds.height);

    for (int i = 0; i < 4; i++) {
        chibi_assert(node->quadrants[i] == NULL);
    }
    chibi_assert_eq_int(0, node->num_elems);
    chibi_assert_eq_int(TRUE, node->is_leaf);
}

CHIBI_TEST(TestInsertElementSimple)
{
    struct Ratr0QuadTreeNode *node = ratr0_new_quad_tree(0, 0, 320, 256);
    struct Ratr0BoundingBox elem = {10, 10, 20, 20};
    ratr0_quadtree_insert(node, &elem);
    chibi_assert_eq_int(1, node->num_elems);
    chibi_assert(&elem == node->elems[0]);
}

CHIBI_TEST(TestInsertClearInsert)
{
    struct Ratr0QuadTreeNode *node = ratr0_new_quad_tree(0, 0, 320, 256);
    struct Ratr0BoundingBox elem = {10, 10, 20, 20};
    ratr0_quadtree_insert(node, &elem);
    chibi_assert_eq_int(1, node->num_elems);
    chibi_assert(&elem == node->elems[0]);
    ratr0_quadtree_clear();
    chibi_assert_eq_int(0, node->num_elems);
    chibi_assert(node->is_leaf);
    ratr0_quadtree_insert(node, &elem);
    chibi_assert_eq_int(1, node->num_elems);
    chibi_assert(&elem == node->elems[0]);
}

CHIBI_TEST(TestBoundingBoxOverlap)
{
    struct Ratr0BoundingBox r1 = {0, 0, 2, 2};
    struct Ratr0BoundingBox r2 = {1, 1, 3, 3};
    struct Ratr0BoundingBox r3 = {5, 5, 3, 3};

    chibi_assert(ratr0_bb_overlap(&r1, &r1));
    chibi_assert(ratr0_bb_overlap(&r1, &r2));
    chibi_assert(!ratr0_bb_overlap(&r1, &r3));
}

CHIBI_TEST(TestFindOverlappingSimple)
{
    struct Ratr0BoundingBox r1 = {0, 0, 2, 2};
    struct Ratr0BoundingBox r2 = {1, 1, 3, 3};
    struct Ratr0QuadTreeNode *node = ratr0_new_quad_tree(0, 0, 320, 256);
    struct Ratr0Vector *result = ratr0_new_vector(10, 4);

    ratr0_quadtree_insert(node, &r1);
    ratr0_quadtree_overlapping(node, &r2, result);
    chibi_assert_eq_int(1, result->num_elements);
    chibi_assert(&r1 == result->elements[0]);
}

CHIBI_TEST(TestFindOverlappingBob)
{
    struct Ratr0BoundingBox r1 = {50, 16, 20, 23};
    struct Ratr0BoundingBox r2 = {83, 32, 20, 23};
    struct Ratr0QuadTreeNode *node = ratr0_new_quad_tree(0, 0, 320, 256);
    struct Ratr0Vector *result = ratr0_new_vector(10, 4);

    ratr0_quadtree_insert(node, &r1);
    ratr0_quadtree_overlapping(node, &r2, result);
    chibi_assert_eq_int(0, result->num_elements);
}

CHIBI_TEST(TestSplitNode)
{
    struct Ratr0QuadTreeNode *node = ratr0_new_quad_tree(0, 0, 320, 256);
    ratr0_quadtree_split_node(node);
    chibi_assert_eq_int(FALSE, node->is_leaf);

    // Quadrant 0
    chibi_assert_eq_int(0, node->quadrants[0]->bounds.x);
    chibi_assert_eq_int(0, node->quadrants[0]->bounds.y);
    chibi_assert_eq_int(160, node->quadrants[0]->bounds.width);
    chibi_assert_eq_int(128, node->quadrants[0]->bounds.height);

    // Quadrant 1
    chibi_assert_eq_int(160, node->quadrants[1]->bounds.x);
    chibi_assert_eq_int(0, node->quadrants[1]->bounds.y);
    chibi_assert_eq_int(160, node->quadrants[1]->bounds.width);
    chibi_assert_eq_int(128, node->quadrants[1]->bounds.height);

    // Quadrant 2
    chibi_assert_eq_int(0, node->quadrants[2]->bounds.x);
    chibi_assert_eq_int(128, node->quadrants[2]->bounds.y);
    chibi_assert_eq_int(160, node->quadrants[2]->bounds.width);
    chibi_assert_eq_int(128, node->quadrants[2]->bounds.height);

    // Quadrant 3
    chibi_assert_eq_int(160, node->quadrants[3]->bounds.x);
    chibi_assert_eq_int(128, node->quadrants[3]->bounds.y);
    chibi_assert_eq_int(160, node->quadrants[3]->bounds.width);
    chibi_assert_eq_int(128, node->quadrants[3]->bounds.height);
}

CHIBI_TEST(TestFindQuadrants)
{
    struct Ratr0QuadTreeNode *node = ratr0_new_quad_tree(0, 0, 320, 256);
    struct Ratr0BoundingBox r1 = {0, 0, 2, 2}; // only in quadrant 0
    struct Ratr0BoundingBox r2 = {10, 110, 20, 30}; // in quadrant 0 and 2
    struct Ratr0BoundingBox r3 = {154, 80, 30, 10}; // in quadrant 0 and 1
    struct Ratr0BoundingBox r4 = {154, 110, 30, 40}; // in all quadrants
    ratr0_quadtree_split_node(node);
    UINT8 results[4];
    UINT8 num_results = ratr0_quadtree_quadrants(node, &r1, results);
    chibi_assert_eq_int(1, num_results);
    chibi_assert_eq_int(0, results[0]);

    num_results = ratr0_quadtree_quadrants(node, &r2, results);
    chibi_assert_eq_int(2, num_results);
    chibi_assert_eq_int(0, results[0]);
    chibi_assert_eq_int(2, results[1]);

    num_results = ratr0_quadtree_quadrants(node, &r3, results);
    chibi_assert_eq_int(2, num_results);
    chibi_assert_eq_int(0, results[0]);
    chibi_assert_eq_int(1, results[1]);

    num_results = ratr0_quadtree_quadrants(node, &r4, results);
    chibi_assert_eq_int(4, num_results);
    chibi_assert_eq_int(0, results[0]);
    chibi_assert_eq_int(1, results[1]);
    chibi_assert_eq_int(2, results[2]);
    chibi_assert_eq_int(3, results[3]);
}

CHIBI_TEST(TestInsertElementOverlapping)
{
    struct Ratr0QuadTreeNode *node = ratr0_new_quad_tree(0, 0, 320, 256);
    ratr0_quadtree_split_node(node);
    struct Ratr0BoundingBox elem = {154, 110, 30, 40}; // in all quadrants
    ratr0_quadtree_insert(node, &elem);
    chibi_assert_eq_int(0, node->num_elems); // It's in all the child nodes
    chibi_assert_eq_int(1, node->quadrants[0]->num_elems);
    chibi_assert(&elem == node->quadrants[0]->elems[0]);
    chibi_assert_eq_int(1, node->quadrants[1]->num_elems);
    chibi_assert(&elem == node->quadrants[1]->elems[0]);
    chibi_assert_eq_int(1, node->quadrants[2]->num_elems);
    chibi_assert(&elem == node->quadrants[2]->elems[0]);
    chibi_assert_eq_int(1, node->quadrants[3]->num_elems);
    chibi_assert(&elem == node->quadrants[3]->elems[0]);
}

CHIBI_TEST(TestInsertElementsWithSplit)
{
    struct Ratr0QuadTreeNode *node = ratr0_new_quad_tree(0, 0, 320, 256);
    struct Ratr0BoundingBox e0 = {10, 20, 10, 10}; // quadrant 0
    struct Ratr0BoundingBox e1 = {180, 20, 10, 10}; // quadrant 1
    struct Ratr0BoundingBox e2 = {10, 150, 10, 10}; // quadrant 2
    struct Ratr0BoundingBox e3 = {180, 150, 10, 10}; // quadrant 3
    struct Ratr0BoundingBox e4 = {16, 24, 10, 10}; // quadrant 0
    struct Ratr0BoundingBox e5 = {190, 24, 10, 10}; // quadrant 1
    ratr0_quadtree_insert(node, &e0);
    ratr0_quadtree_insert(node, &e1);
    ratr0_quadtree_insert(node, &e2);
    ratr0_quadtree_insert(node, &e3);
    ratr0_quadtree_insert(node, &e4);
    ratr0_quadtree_insert(node, &e5);
    chibi_assert_eq_int(6, node->num_elems); // That's the max without split

    // first split
    struct Ratr0BoundingBox e6 = {30, 182, 10, 10}; // quadrant 2
    ratr0_quadtree_insert(node, &e6);
    chibi_assert_eq_int(0, node->is_leaf); // not a leaf anymore
    chibi_assert_eq_int(0, node->num_elems); // elememts moved to quadrants
    chibi_assert_eq_int(2, node->quadrants[0]->num_elems);
    chibi_assert_eq_int(2, node->quadrants[1]->num_elems);
    chibi_assert_eq_int(2, node->quadrants[2]->num_elems);
    chibi_assert_eq_int(1, node->quadrants[3]->num_elems);
}

/*
 * SUITE DEFINITION
 */

chibi_suite *CoreSuite(void)
{
    chibi_suite *suite = chibi_suite_new_fixture("ratr0.QuadTreeSuite", quadtreetest_setup,
                                                 quadtreetest_teardown, NULL);
    chibi_suite_add_test(suite, TestCreateQuadTree);
    chibi_suite_add_test(suite, TestInsertElementSimple);
    chibi_suite_add_test(suite, TestBoundingBoxOverlap);
    chibi_suite_add_test(suite, TestFindOverlappingSimple);
    chibi_suite_add_test(suite, TestFindOverlappingBob);
    chibi_suite_add_test(suite, TestFindQuadrants);
    chibi_suite_add_test(suite, TestSplitNode);
    chibi_suite_add_test(suite, TestInsertElementOverlapping);
    chibi_suite_add_test(suite, TestInsertElementsWithSplit);
    chibi_suite_add_test(suite, TestInsertClearInsert);

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
