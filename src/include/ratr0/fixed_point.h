#ifndef __FIXED_POINT_H__
#define __FIXED_POINT_H__
#include <ratr0/data_types.h>

typedef INT32 FIXED16;
typedef INT32 FIXED8;

/*
 * 32 bit fixed point math, using 24.8 representation. The range of the fractional part
 * extends from 0-99.
 * The accuracy is 0.02, which means the fractional part can occasionally
 * be off by up to 2
 *
 * For now, only addition/subtraction is tested.
 */
#define FIXED8_SHIFT (8)
#define FIXED8_MASK ((1 << FIXED8_SHIFT) - 1)
#define FIXED8_SCALE (100)

/*
  The amount before the decimal point (integral part and fractional parts)
  are obtained with the macros below. Note that negative numbers need
  to be converted to their absolute equivalent before using this
*/
#define FIXED8_INT_ABS(f) (f >> FIXED8_SHIFT)
#define FIXED8_FRAC_ABS(f) ((f & (FIXED8_MASK)) * FIXED8_SCALE / FIXED8_MASK)
#define FIXED8_CREATE_ABS(i, f) ((i << FIXED8_SHIFT) | (((f << FIXED8_SHIFT) / FIXED8_SCALE) & FIXED8_MASK))

/**
 * These are the functions that should be used to create all fixed point numbers.
 */
#define FIXED8_INT(f) (f < 0 ? -FIXED8_INT_ABS((~f + 1)) : FIXED8_INT_ABS(f))
#define FIXED8_FRAC(f) (f < 0 ? FIXED8_FRAC_ABS((~f + 1)) : FIXED8_FRAC_ABS(f))
#define FIXED8_CREATE(i, f) (i < 0 ? 0xffffffff - FIXED8_CREATE_ABS(-i, f) : FIXED8_CREATE_ABS(i, f))

#define FIXED8_MUL(f1, f2) (((INT32) (f1 * f2)) >> FIXED8_SHIFT)

/* 16:16 Representation. This reprensentation supports 3 digits behind the decimal point*/

#define FIXED16_SHIFT (16)
#define FIXED16_MASK ((1 << FIXED16_SHIFT) - 1)
#define FIXED16_SCALE (1000)

/*
  The amount before the decimal point (integral part and fractional parts)
  are obtained with the macros below. Note that negative numbers need
  to be converted to their absolute equivalent before using this
*/
#define FIXED16_INT_ABS(f) (f >> FIXED16_SHIFT)
#define FIXED16_FRAC_ABS(f) ((f & (FIXED16_MASK)) * FIXED16_SCALE / FIXED16_MASK)
#define FIXED16_CREATE_ABS(i, f) ((i << FIXED16_SHIFT) | (((f << FIXED16_SHIFT) / FIXED16_SCALE) & FIXED16_MASK))

/**
 * These are the functions that should be used to create all fixed point numbers.
 */
#define FIXED16_INT(f) (f < 0 ? -FIXED16_INT_ABS((~f + 1)) : FIXED16_INT_ABS(f))
#define FIXED16_FRAC(f) (f < 0 ? FIXED16_FRAC_ABS((~f + 1)) : FIXED16_FRAC_ABS(f))
#define FIXED16_CREATE(i, f) (i < 0 ? 0xffffffff - FIXED16_CREATE_ABS(-i, f) : FIXED16_CREATE_ABS(i, f))

#define FIXED16_MUL(f1, f2) (((INT32) (f1 * f2)) >> FIXED16_SHIFT)

#endif /* __FIXED_POINT_H__ */
