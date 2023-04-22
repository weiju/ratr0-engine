#pragma once
#ifndef __RATR0_BITSET_H__
#define __RATR0_BITSET_H__

#include <ratr0/data_types.h>
/**
 * An efficient way to implement a dirty rectangle set. Due to the fixed grid
 * structrure of a tile map / backdrop and due to the fact we only need to answer
 * a yes/no question, we can map each tile to a single bit.
 * Insertion and lookup are O(1). Clearing and iterating the set takes longer because
 * it is dependent on the words in the the bit set.
 */
void ratr0_bitset_clear(UINT32 *bitset, UINT16 size);
void ratr0_bitset_insert(UINT32 *bitset, UINT16 size, UINT16 index);
BOOL ratr0_bitset_isset(UINT32 *bitset, UINT16 size, UINT16 index);

/**
 * Iterates over all set bits in the set
 */
void ratr0_bitset_iterate(UINT32 *bitset, UINT16 size, void (*func)(UINT16));

#endif /* __RATR0_BITSET_H__ */
