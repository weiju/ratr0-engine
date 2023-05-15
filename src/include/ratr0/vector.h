/** @file vector.h
 *
 * This module implements vectors (dynamic array lists).
 * Vectors expand when they run out of storage.
 * Random access is O(1).
 * Append is O(1) except when expansion is needed. In that
 * case the operation is O(number of elements) because of
 * the copy operation
 */
#pragma once
#ifndef __RATR0_VECTOR_H__
#define __RATR0_VECTOR_H__

#include <ratr0/data_types.h>
#include <ratr0/engine.h>
#include <ratr0/memory.h>

/**
 * A data structure that represents a dynamic array list. This array will
 * increase in size whenever its capacity is exceeded.
 */
struct Ratr0Vector {
    /** \brief array holding the elements */
    void **elements;
    /** \brief size of the elements array */
    UINT16 capacity;
    /** \brief resize by this many elements when capacity is exhausted */
    UINT16 resize_by;
    /** \brief number of actual elements in the array */
    UINT16 num_elements;
    /** \brief memory handle to the memory subsystem */
    Ratr0MemHandle handle;
};

/**
 * Initializes the vector module.
 *
 * @param engine the Ratr0Engine instance
 */
extern void ratr0_vector_startup(Ratr0Engine *engine);

/**
 * Shuts down the vector module.
 */
extern void ratr0_vector_shutdown(void);

/**
 * Creates a new vector object.
 *
 * @param initial_capacity initial capacity
 * @param resize_by add this many elements if capacity is exceeded
 * @return a new, initialized vector
 */
extern struct Ratr0Vector *ratr0_new_vector(UINT16 initial_capacity, UINT16 resize_by);

/**
 * Appends an element to the end of the vector. The vector will expand if its
 * current capacity is exceeded.
 *
 * @param vec the vector to append to
 * @param elem the element to append
 */
extern void ratr0_vector_append(struct Ratr0Vector *vec, void *elem);

#endif /* __RATR0_VECTOR_H__ */
