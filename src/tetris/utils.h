#pragma once
#ifndef __TETRAZONE_UTILS_H__
#define __TETRAZONE_UTILS_H__

#include <ratr0/ratr0.h>
/**
 * Helper functions for Tetra Zone. Useful for extracting digits from the
 * score and level values to convert into the display
 */
UINT8 num_digits(UINT16 value);
UINT8 extract_digits(UINT8 *digit_buffer, UINT8 max_digits, UINT16 value);

#endif // !__TETRAZONE_UTILS_H__
