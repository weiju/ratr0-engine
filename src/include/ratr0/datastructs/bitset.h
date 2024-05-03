/** @file bitset.h
 *
 * An efficient way to implement a dirty rectangle set. Due to the fixed grid
 * structrure of a tile map / backdrop and due to the fact we only need to answer
 * a yes/no question, we can map each tile to a single bit.
 * Insertion and lookup are O(1). Clearing and iterating the set takes longer because
 * it is dependent on the words in the the bit set.
 */
#pragma once
#ifndef __RATR0_BITSET_H__
#define __RATR0_BITSET_H__

#include <ratr0/data_types.h>

/**
 * Remove all elements of the specified bit set.
 *
 * @param bitset a bitset, stored as an array of 32 bit unsigned integers
 * @param size the size of the bitset array
 */
void ratr0_bitset_clear(UINT32 *bitset, UINT16 size);

/**
 * Add an element to the specified bit set.
 *
 * @param bitset a bitset, stored as an array of 32 bit unsigned integers
 * @param size the size of the bitset array
 * @param index index of the element to add
 */
void ratr0_bitset_insert(UINT32 *bitset, UINT16 size, UINT16 index);

/**
 * Check if an element exists in the specified bit set.
 *
 * @param bitset a bitset, stored as an array of 32 bit unsigned integers
 * @param size the size of the bitset array
 * @param index index of the element to add
 * @return TRUE if element exists, FALSE otherwise
 */
BOOL ratr0_bitset_isset(UINT32 *bitset, UINT16 size, UINT16 index);

/**
 * Iterates over all set bits in the set.
 *
 * @param bitset a bitset, stored as an array of 32 bit unsigned integers
 * @param size the size of the bitset array
 * @param func a function that is called with the index of the element
 */
void ratr0_bitset_iterate(UINT32 *bitset, UINT16 size, void (*func)(UINT16));

#endif /* __RATR0_BITSET_H__ */
