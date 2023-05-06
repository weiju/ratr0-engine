#pragma once
#ifndef __RATR0_TREESET_H__
#define __RATR0_TREESET_H__
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

struct TreeSetNode {
    // RB Tree node attributes
    struct TreeSetNode *left, *right, *parent;
    UINT16 color;

    // payload
    void *value;
};

#define MAX_TREESET_NODES (50)

struct TreeSet {
    struct TreeSetNode *root, *NIL;
    UINT16 num_elements; // the current elements in the array
    struct TreeSetNode nodes[MAX_TREESET_NODES];
    int next_node;
};

struct TreeSets {
    void (*shutdown)(void);
    // only one instance at the moment
    struct TreeSet *(*get_tree_set)(void);
};

extern struct TreeSets *ratr0_startup_tree_sets(Ratr0Engine *engine);

/**
 * We only need 3 operations: insert, iteration and empty the entire thing.
 */
extern BOOL tree_set_insert(struct TreeSet *set, void *value,
                            BOOL (*lt)(void *, void *),
                            BOOL (*eq)(void *, void *));

extern void tree_set_iterate(struct TreeSet *set,
                             void (*process_node)(struct TreeSetNode *, void *),
                             void *user_data);
extern void tree_set_clear(struct TreeSet *set);
#endif /* __RATR0_TREESET_H__ */
