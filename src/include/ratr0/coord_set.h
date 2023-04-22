#pragma once
#ifndef __RATR0_COORD_SET_H__
#define __RATR0_COORD_SET_H__
#include <ratr0/data_types.h>
#include <ratr0/memory.h>

/**
 * Representation of a coordinate set.
 * We need coordinate sets for representing collections of unique values, such as
 * dirty rectangles.
 * Coordinate sets are implemented as red black trees so we can have relatively fast
 * operations.
 */
enum {RBT_RED=1, RBT_BLACK};

struct Coord {
    // RB Tree node attributes
    struct Coord *left, *right, *parent;
    UINT16 color;

    // payload
    INT16 x, y;
};

struct CoordSet {
    struct Coord *root;
    UINT16 num_elements; // the current elements in the array
};

struct CoordSets {
    void (*shutdown)(void);
    // only one instance at the monent
    struct CoordSet *(*get_coord_set)(void);
};

extern struct CoordSets *ratr0_startup_coord_sets(Ratr0Engine *engine);

/**
 * We only need 3 operations: insert, iteration and empty the entire thing.
 */
extern BOOL coord_set_insert(struct CoordSet *set, UINT16 x, UINT16 y);
extern void coord_set_iterate(struct CoordSet *set,
                              void (*process_coord)(struct Coord *, void *), void *user_data);
extern void coord_set_clear(struct CoordSet *set);
#endif /* __RATR0_COORD_SET_H__ */

