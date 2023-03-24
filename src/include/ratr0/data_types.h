#pragma once
#ifndef __RATR0_DATA_TYPES_H__
#define __RATR0_DATA_TYPES_H__

#ifdef __VBCC__
typedef int INT32;
typedef char BOOL;

#define TRUE (1)
#define FALSE (0)

#else
#include <stdint.h>
typedef int32_t INT32;
typedef int16_t BOOL;

#define TRUE (1)
#define FALSE (0)
#endif

/*
 * RATR0 Engine basic data types. Makes sure we are operating on the
 * same size of data independent of system and compiler.
 */

#endif /* __RATR0_DATA_TYPES_H__ */
