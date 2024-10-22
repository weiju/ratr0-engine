/** @file queue.h
 *
 * This module implements queues that are represented by circular arrays
 * and are created at compile time through C macros.
 * Enqueue and dequeue operations are O(1).
 */
#pragma once
#ifndef __RATR0_QUEUE_H__
#define __RATR0_QUEUE_H__

#include <ratr0/data_types.h>

/**
 * Create array of queues. This can be used e.g. as a render queue
 * for multi-buffer. Since this macro creates an initialization function
 * init_<name>_queues() for the queue array it has to be used outside of
 * function scope.
 *
 * The following variables will exist after calling this macro:
 *   * <name>
 *   * init<name>_queues()
 *   * <name>_size
 *   * <name>_first
 *   * <name>_num_elems
 *
 * It is recommended to call init_<name>_queues() to properly initialize
 * the queue array before using it.
 *
 * @param name Name of the queue
 * @param type Type of the elements
 * @param size Maximum number of elements
 * @param init_elem Function that initializes an element
 * @param arr_size Maximum number of queues
 */
#define RATR0_QUEUE_ARR(name, type, size, arr_size) \
    const UINT16 name ## _size = size; \
    type name[arr_size][size]; \
    UINT16 name ## _first[arr_size]; \
    UINT16 name ## _num_elems[arr_size];                  \
    void init_ ## name ## _queues(void) {          \
        for (int i = 0; i < arr_size; i++) { \
            name ## _first[i] = 0; \
            name ## _num_elems[i] = 0; \
        } \
        memset((void *) &name, '\0', arr_size * size * sizeof(type)); \
    } // end of queue array macro

/**
 * Defines the elements of a RATR0_QUEUE_ARR. This is intended for
 * header files.
 *
 * @param name Name of the queue
 * @param type Type of the elements
 * @param size Maximum number of elements
 * @param arr_size Maximum number of queues
 */
#define RATR0_QUEUE_ARR_DEF(name, type, size, arr_size) \
    extern const UINT16 name ## _size; \
    extern type name[arr_size][size]; \
    extern UINT16 name ## _first[arr_size]; \
    extern UINT16 name ## _num_elems[arr_size];                  \

/**
 * Enqueue an element in a queue within the queue array. Note that there
 * is no limit check for adding more elements than the maximum size.
 *
 * @param q Name of the queue array
 * @param i Index of the queue
 * @param elem Element to enqueue
 */
#define RATR0_ENQUEUE_ARR(q, i, elem) \
    q[i][(q ## _first[i] + q ## _num_elems[i]) % q ## _size] = elem; \
    q ## _num_elems[i]++; // end of enqueue array macro


/**
 * Dequeue an element from a queue within the queue array. Note that there
 * is no limit check for dequeuing more elements than available. The
 * result is stored in the "result" parameter.
 *
 * @param result Result object
 * @param q Name of the queue array
 * @param i Index of the queue
 */
#define RATR0_DEQUEUE_ARR(result, q, i)  \
    result = q[i][q ## _first[i]];                       \
    q ## _first[i] = (q ## _first[i] + 1) % q ## _size ; \
    q ## _num_elems[i]--; // end of dequeue array macro

#endif /* __RATR0_QUEUE_H__ */
