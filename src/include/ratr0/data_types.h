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
typedef int INT32;
typedef unsigned int UINT32;
typedef short INT16;
typedef unsigned short UINT16;
typedef char INT8;
typedef unsigned char UINT8;

typedef char CHAR;

#else

#include <stdint.h>
typedef int32_t INT32;
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

/**
 * Nodes are the base object of the system
 * Top level node that is the base of a node. The node system is inspired by
 * the Godot design, but will be much simpler to accomodate to the target systems
 * that are very memory constrained.
 * In general, nodes can call methods on nodes that are below them in the scene tree,
 * while signal are used to communicate up and across the hierarchy.
 */
struct Ratr0Scene;
struct Ratr0Node {
    /* Identifying information. We don't support subclassing. It's rather a way to find the
     * appropriate handlers */
    UINT16 class_id;

    struct Ratr0Node *next, *children;

    void (*update)(struct Ratr0Scene *scene, struct Ratr0Node *this_node);
};

enum { RATR0_NODE, BACKGROUND, ANIM_SPRITE2D, AMIGA_SPRITE, AMIGA_BOB};

#endif /* __RATR0_DATA_TYPES_H__ */
