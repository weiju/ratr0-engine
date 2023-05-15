#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ratr0/memory.h>
#include <ratr0/vector.h>
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

void vectortest_setup(void *userdata)
{
    num_mem_entries = 0;
    memsys.allocate_block = &mock_allocate_block;
    memsys.free_block = &mock_free_block;
    memsys.block_address = &mock_block_address;
    mock_engine.memory_system = &memsys;
    ratr0_vector_startup(&mock_engine);
}

void vectortest_teardown(void *userdata) {
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
CHIBI_TEST(TestNewVector)
{
    struct Ratr0Vector *vec = ratr0_new_vector(10, 4);
    chibi_assert_eq_int(0, vec->num_elements);
    chibi_assert_eq_int(10, vec->capacity);
    chibi_assert_eq_int(4, vec->resize_by);
}

CHIBI_TEST(TestAppendElement)
{
    struct Ratr0Vector *vec = ratr0_new_vector(10, 4);
    struct Ratr0BoundingBox bbox = { 0, 0, 1, 12};
    ratr0_vector_append(vec, &bbox);
    chibi_assert_eq_int(1, vec->num_elements);
    chibi_assert(vec->elements[0] == &bbox);
}

CHIBI_TEST(TestAppendAndResize)
{
    struct Ratr0Vector *vec = ratr0_new_vector(2, 2);
    struct Ratr0BoundingBox bbox1 = { 0, 0, 1, 12};
    struct Ratr0BoundingBox bbox2 = { 3, 1, 2, 4};
    struct Ratr0BoundingBox bbox3 = { 13, 11, 12, 14};
    ratr0_vector_append(vec, &bbox1);
    ratr0_vector_append(vec, &bbox2);
    chibi_assert_eq_int(2, vec->num_elements);
    chibi_assert(vec->elements[0] == &bbox1);
    chibi_assert(vec->elements[1] == &bbox2);

    // Now do an insert that extends the array
    ratr0_vector_append(vec, &bbox3);
    chibi_assert_eq_int(4, vec->capacity);
    chibi_assert_eq_int(3, vec->num_elements);
    chibi_assert(vec->elements[0] == &bbox1);
    chibi_assert(vec->elements[1] == &bbox2);
    chibi_assert(vec->elements[2] == &bbox3);
}

/*
 * SUITE DEFINITION
 */

chibi_suite *CoreSuite(void)
{
    chibi_suite *suite = chibi_suite_new_fixture("ratr0.VectorSuite", vectortest_setup,
                                                 vectortest_teardown, NULL);
    chibi_suite_add_test(suite, TestNewVector);
    chibi_suite_add_test(suite, TestAppendElement);
    chibi_suite_add_test(suite, TestAppendAndResize);

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
