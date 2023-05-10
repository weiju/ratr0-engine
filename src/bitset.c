/** @file bitset.c */
#include <ratr0/bitset.h>
#include <string.h>
#include <stdio.h>

void ratr0_bitset_clear(UINT32 *bitset, UINT16 size)
{
    memset(bitset, 0, sizeof(UINT32) * size);
}

void ratr0_bitset_insert(UINT32 *bitset, UINT16 size, UINT16 index)
{
    bitset[index >> 5] |= 1 << (31 - index & 31);
}

BOOL ratr0_bitset_isset(UINT32 *bitset, UINT16 size, UINT16 index)
{
    UINT32 bitmask = 1 << (31 - index & 31);
    return (bitset[index >> 5] & bitmask) == bitmask;
}

/**
 * Iterates over all set bits in the set
 */
void ratr0_bitset_iterate(UINT32 *bitset, UINT16 size, void (*func)(UINT16))
{
    int i, j, k;
    UINT8 *bytes, mask, b;
    for (i = 0; i < size; i++) {
        // We can immediately skip over empty words
        if (bitset[i] != 0) {
            // now we can split each UINT32 into UINT8s and do the same
            // make sure we are aware of endianess, because most modern architectures
            // are little endian
            bytes = (UINT8 *) &bitset[i];
            for (j = 0; j < 4; j++) {
#ifdef LITTLE_ENDIAN
                // I hate little endian !!!
                if (bytes[3-j] != 0) {
                    b = bytes[3-j];
#else
                // see how naturally bytes advance in big endian ?
                if (bytes[j] != 0) {
                    b = bytes[j];
#endif
                    for (k = 0; k < 8; k++) {
                        mask = 1 << (7 - k);
                        if ((b & mask) == mask) {
                            // call func with index
                            func((i << 5) + (j << 3) + k);
                        }
                    }
                }
            }
        }
    }
}
