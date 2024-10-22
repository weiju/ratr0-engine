#include <string.h>
#include <ratr0/datastructs/queue.h>
#include "../../chibi_test/chibi.h"

void queue_test_setup(void *userdata) { }
void queue_test_teardown(void *userdata) { }

struct MyStruct {
    int a, b, c;
};

#define ARR_SIZE 2
#define QUEUE_SIZE 3

RATR0_QUEUE_ARR(myqueue, struct MyStruct, QUEUE_SIZE, 2);

/*
 * TEST CASES
 */

/* Check that the queue is initialized */
CHIBI_TEST(TestRatr0QueueArrInit)
{
    init_myqueue_queues();
    chibi_assert_eq_int(QUEUE_SIZE, myqueue_size);
    chibi_assert_eq_int(0, myqueue_first[0]);
    chibi_assert_eq_int(0, myqueue_num_elems[0]);
    chibi_assert_eq_int(0, myqueue_first[1]);
    chibi_assert_eq_int(0, myqueue_num_elems[1]);
}

CHIBI_TEST(TestRatr0QueueArrEnqueue)
{
    struct MyStruct s1 = {1, 2, 3};
    struct MyStruct s2 = {3, 4, 5};
    struct MyStruct s3 = {4, 5, 6};
    init_myqueue_queues();
    RATR0_ENQUEUE_ARR(myqueue, 0, s1);
    RATR0_ENQUEUE_ARR(myqueue, 0, s2);
    RATR0_ENQUEUE_ARR(myqueue, 0, s3);

    chibi_assert_eq_int(0, myqueue_first[0]);
    chibi_assert_eq_int(3, myqueue_num_elems[0]);
    chibi_assert_eq_int(0, myqueue_first[1]);
    chibi_assert_eq_int(0, myqueue_num_elems[1]);
}

CHIBI_TEST(TestRatr0QueueArrDequeue)
{
    struct MyStruct s1 = {1, 2, 3};
    struct MyStruct s2 = {3, 4, 5};
    struct MyStruct s3 = {4, 5, 6};
    struct MyStruct dequeued;
    init_myqueue_queues();
    RATR0_ENQUEUE_ARR(myqueue, 0, s1);
    RATR0_ENQUEUE_ARR(myqueue, 0, s2);
    RATR0_ENQUEUE_ARR(myqueue, 0, s3);
    RATR0_DEQUEUE_ARR(dequeued, myqueue, 0);

    chibi_assert_eq_int(1, myqueue_first[0]);
    chibi_assert_eq_int(2, myqueue_num_elems[0]);

    // check the element
    chibi_assert_eq_int(s1.a, dequeued.a);
    chibi_assert_eq_int(s1.b, dequeued.b);
    chibi_assert_eq_int(s1.c, dequeued.c);
}

CHIBI_TEST(TestRatr0QueueArrEnqueueWrapped)
{
    struct MyStruct s1 = {1, 2, 3};
    struct MyStruct s2 = {3, 4, 5};
    struct MyStruct s3 = {4, 5, 6};
    struct MyStruct dequeued;
    init_myqueue_queues();
    RATR0_ENQUEUE_ARR(myqueue, 0, s1);
    RATR0_ENQUEUE_ARR(myqueue, 0, s2);
    RATR0_ENQUEUE_ARR(myqueue, 0, s3);
    RATR0_DEQUEUE_ARR(dequeued, myqueue, 0);

    RATR0_ENQUEUE_ARR(myqueue, 0, s3);
    chibi_assert_eq_int(1, myqueue_first[0]);
    chibi_assert_eq_int(3, myqueue_num_elems[0]);

    // test the wrapped element
    chibi_assert_eq_int(s3.a, myqueue[0][0].a);
    chibi_assert_eq_int(s3.b, myqueue[0][0].b);
    chibi_assert_eq_int(s3.c, myqueue[0][0].c);
}

/*
 * SUITE DEFINITION
 */

chibi_suite *build_suite(void)
{
    chibi_suite *suite = chibi_suite_new_fixture("ratr0.QueueSuite",
                                                 queue_test_setup,
                                                 queue_test_teardown, NULL);
    chibi_suite_add_test(suite, TestRatr0QueueArrInit);
    chibi_suite_add_test(suite, TestRatr0QueueArrEnqueue);
    chibi_suite_add_test(suite, TestRatr0QueueArrDequeue);
    chibi_suite_add_test(suite, TestRatr0QueueArrEnqueueWrapped);

    return suite;
}

int main(int argc, char **argv)
{
    chibi_summary_data summary;
    chibi_suite *suite = build_suite();

    chibi_suite_run(suite, &summary);
    chibi_suite_delete(suite);
    return summary.num_failures;
}
