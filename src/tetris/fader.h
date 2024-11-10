#pragma once
#ifndef __TETRAZONE_FADER_H__
#define __TETRAZONE_FADER_H__

#include <ratr0/ratr0.h>

/**
 * Implements a timer based fader. The idea is to have a
 * structure that defines a palette animation that was precomputed
 * and the function will set the copper list based on that structure
 */
struct FaderData {
    UINT8 num_colors;
    UINT8 num_values;
    /**
     * a 2D array, first dimension is the palette entry numbers
     * the second dimension is the values
     */
    UINT16 **color_values;
};

#endif // !__TETRAZONE_FADER_H__
