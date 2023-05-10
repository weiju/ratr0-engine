/** @file fixed_point.h */
#pragma once
#ifndef __FIXED_POINT_H__
#define __FIXED_POINT_H__
#include <ratr0/data_types.h>

/**
 * \var typedef INT32 FIXED16
 * \brief A type definition for 16.16 fixed point numbers.
 */
typedef INT32 FIXED16;

/**
 * \var typedef INT32 FIXED8
 * \brief A type definition for 24.8 fixed point numbers.
 */
typedef INT32 FIXED8;

//
// 32 bit fixed point math, using 24.8 representation. The range of the fractional part
// extends from 0-99.
// The accuracy is 0.02, which means the fractional part can occasionally
// be off by up to 2
//
// For now, only addition/subtraction is tested.
//

/** \brief Shift amount for 24.8 fixed point numbers */
#define FIXED8_SHIFT (8)
/** \brief Mask for 24.8 fixed point numbers */
#define FIXED8_MASK ((1 << FIXED8_SHIFT) - 1)
/** \brief Scale factor for  24.8 fixed point numbers */
#define FIXED8_SCALE (100)

/*
  The amount before the decimal point (integral part and fractional parts)
  are obtained with the macros below. Note that negative numbers need
  to be converted to their absolute equivalent before using this
*/
/**
 * \def FIXED8_INT_ABS(f)
 * \brief extract the absolute integer part from the 24.8 fixed point number \a f.
 */
#define FIXED8_INT_ABS(f) (f >> FIXED8_SHIFT)
/**
 * \def FIXED8_FRAC_ABS(f)
 * \brief extract the absolute fractional part from the 24.8 fixed point number \a f.
 */
#define FIXED8_FRAC_ABS(f) ((f & (FIXED8_MASK)) * FIXED8_SCALE / FIXED8_MASK)

/**
 * \def FIXED8_CREATE_ABS(i, f)
 * \brief create 24.8 fixed point number with the integer \a i and fraction \a f.
 */
#define FIXED8_CREATE_ABS(i, f) ((i << FIXED8_SHIFT) | (((f << FIXED8_SHIFT) / FIXED8_SCALE) & FIXED8_MASK))

//
// These are the functions that should be used to create all fixed point numbers.
//
/**
 * \def FIXED8_INT(f)
 * \brief extract the signed integer part from the 24.8 fixed point number \a f.
 */
#define FIXED8_INT(f) (f < 0 ? -FIXED8_INT_ABS((~f + 1)) : FIXED8_INT_ABS(f))

/**
 * \def FIXED8_FRAC(f)
 * \brief extract the fractional part from the 24.8 signed fixed point number \a f.
 */
#define FIXED8_FRAC(f) (f < 0 ? FIXED8_FRAC_ABS((~f + 1)) : FIXED8_FRAC_ABS(f))

/**
 * \def FIXED8_CREATE(i, f)
 * \brief create 24.8 signed fixed point number with the integer \a i and fraction \a f.
 */
#define FIXED8_CREATE(i, f) (i < 0 ? 0xffffffff - FIXED8_CREATE_ABS(-i, f) : FIXED8_CREATE_ABS(i, f))

/**
 * \def FIXED8_MUL(i, f)
 * \brief multiply the 2 24.8 signed fixed point numbers \a f1 and \a f2.
 */
#define FIXED8_MUL(f1, f2) (((INT32) (f1 * f2)) >> FIXED8_SHIFT)

/* 16:16 Representation. This reprensentation supports 3 digits behind the decimal point*/

/** \brief Shift amount for 16.16 fixed point numbers */
#define FIXED16_SHIFT (16)
/** \brief Mask for 16.16 fixed point numbers */
#define FIXED16_MASK ((1 << FIXED16_SHIFT) - 1)
/** \brief Scale factor for  16.16 fixed point numbers */
#define FIXED16_SCALE (1000)

//
//  The amount before the decimal point (integral part and fractional parts)
//  are obtained with the macros below. Note that negative numbers need
//  to be converted to their absolute equivalent before using this
//

/**
 * \def FIXED16_INT_ABS(f)
 * \brief extract the absolute integer part from the 16.16 fixed point number \a f.
 */
#define FIXED16_INT_ABS(f) (f >> FIXED16_SHIFT)
/**
 * \def FIXED16_FRAC_ABS(f)
 * \brief extract the absolute fractional part from the 16.16 fixed point number \a f.
 */
#define FIXED16_FRAC_ABS(f) ((f & (FIXED16_MASK)) * FIXED16_SCALE / FIXED16_MASK)
/**
 * \def FIXED16_CREATE_ABS(i, f)
 * \brief create 16.16 fixed point number with the integer \a i and fraction \a f.
 */
#define FIXED16_CREATE_ABS(i, f) ((i << FIXED16_SHIFT) | (((f << FIXED16_SHIFT) / FIXED16_SCALE) & FIXED16_MASK))

//
// These are the functions that should be used to create all fixed point numbers.
//

/**
 * \def FIXED16_INT(f)
 * \brief extract the signed integer part from the 16.16 fixed point number \a f.
 */
#define FIXED16_INT(f) (f < 0 ? -FIXED16_INT_ABS((~f + 1)) : FIXED16_INT_ABS(f))

/**
 * \def FIXED16_FRAC(f)
 * \brief extract the fractional part from the 16.16 signed fixed point number \a f.
 */
#define FIXED16_FRAC(f) (f < 0 ? FIXED16_FRAC_ABS((~f + 1)) : FIXED16_FRAC_ABS(f))

/**
 * \def FIXED16_CREATE(i, f)
 * \brief create 16.16 signed fixed point number with the integer \a i and fraction \a f.
 */
#define FIXED16_CREATE(i, f) (i < 0 ? 0xffffffff - FIXED16_CREATE_ABS(-i, f) : FIXED16_CREATE_ABS(i, f))

/**
 * \def FIXED16_MUL(i, f)
 * \brief multiply the 2 16.16 signed fixed point numbers \a f1 and \a f2.
 */
#define FIXED16_MUL(f1, f2) (((INT32) (f1 * f2)) >> FIXED16_SHIFT)

#endif /* __FIXED_POINT_H__ */
