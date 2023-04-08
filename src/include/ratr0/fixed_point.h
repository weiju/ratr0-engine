#ifndef __RATR0_FIXED_POINT_H__
#define __RATR0_FIXED_POINT_H__

#include <ratr0/data_types.h>

typedef INT32 FIXED32;

/*
 * 32 bit fixed point math, using 24.8 representation. For now,
 * only addition/subtraction is tested
 */
#define FIXED32_SHIFT (8)
#define FIXED32_MASK (0xff)

/*
  The amount before the decimal point (integral part and fractional parts)
  are obtained with the macros below. Note that negative numbers need
  to be converted to their absolute equivalent before using this
*/
#define FIXED32_INT_ABS(f) (f >> FIXED32_SHIFT)
#define FIXED32_FRAC_ABS(f) ((f & (FIXED32_MASK)) * 100 / FIXED32_MASK)
#define FIXED32_INT(f) (f < 0 ? -FIXED32_INT_ABS((~f + 1)) : FIXED32_INT_ABS(f))
#define FIXED32_FRAC(f) (f < 0 ? FIXED32_FRAC_ABS((~f + 1)) : FIXED32_FRAC_ABS(f))
#define FIXED32_CREATE_ABS(i, f) ((i << FIXED32_SHIFT) | (((f * 256) / 100) & FIXED32_MASK))
#define FIXED32_CREATE(i, f) (i < 0 ? 0xffffffff - FIXED32_CREATE_ABS(-i, f) : FIXED32_CREATE_ABS(i, f))
#define FIXED32_MUL(f1, f2) (((INT32) (f1 * f2)) >> FIXED32_SHIFT)

#endif /* __RATR0_FIXED_POINT_H__ */
