/** @file data_types.h */
#pragma once
#ifndef __RATR0_DATA_TYPES_H__
#define __RATR0_DATA_TYPES_H__

/*
 * RATR0 Engine basic data types. Makes sure we are operating on the
 * same size of data independent of system and compiler.
 */

#ifdef __VBCC__
/*
 * We only use the BOOL definitions from here, while other primitives use
 * a more generic name.
 */
#include <exec/types.h>
/**
 * \var typedef int INT32
 * \brief A type definition for signed 32 bit numbers.
 */
typedef int INT32;
/**
 * \var typedef unsigned int UINT32
 * \brief A type definition for unsigned 32 bit numbers.
 */
typedef unsigned int UINT32;
/**
 * \var typedef short INT16
 * \brief A type definition for signed 16 bit numbers.
 */
typedef short INT16;
/**
 * \var typedef unsigned short UINT16
 * \brief A type definition for unsigned 16 bit numbers.
 */
typedef unsigned short UINT16;
/**
 * \var typedef char INT8
 * \brief A type definition for signed 8 bit numbers.
 */
typedef char INT8;
/**
 * \var typedef unsigned char UINT8
 * \brief A type definition for unsigned 8 bit numbers.
 */
typedef unsigned char UINT8;

/**
 * \var typedef char CHAR
 * \brief A type definition for 8 bit characters.
 */
typedef char CHAR;

#else

#include <stdint.h>

/**
 * \var typedef int32_t INT32
 * \brief A type definition for signed 32 bit numbers.
 */
typedef int32_t INT32;
/**
 * \var typedef uint32_t UINT32
 * \brief A type definition for unsigned 32 bit numbers.
 */
typedef uint32_t UINT32;
typedef int16_t INT16;
typedef uint16_t UINT16;
typedef int8_t INT8;
typedef uint8_t UINT8;

typedef char CHAR;
typedef int16_t BOOL;

#define TRUE (1)
#define FALSE (0)
#ifndef NULL
#define NULL (0)
#endif

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN
#endif /* LITTLE_ENDIAN */

#endif /* VBCC */


#endif /* __RATR0_DATA_TYPES_H__ */
