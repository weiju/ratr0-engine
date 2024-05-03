/** @file treeset.h
 *
 * Representation of a set containing generic pointers. In RATR0 this is mainly
 * used to store active BOBs.
 *
 * Tree sets are implemented as red black trees so we can have relatively fast
 * operations. This specific implementation is designed so that clearing a set
 * is O(1), with just the setting of an array index. This is so we can reuse the
 * same set over all iterations of a game loop without worrying about memory
 * allocation issues.
 */
#pragma once
#ifndef __RATR0_TREESET_H__
#define __RATR0_TREESET_H__
#include <ratr0/data_types.h>
#include <ratr0/memory.h>

/** \brief node colors for the tree set */
enum Ratr0TreeSetNodeColor {RBT_RED=1, RBT_BLACK};

/**
 * Representation of a tree set node.
 */
struct Ratr0TreeSetNode {
    /** \brief left child */
    struct Ratr0TreeSetNode *left;
    /** \brief right child */
    struct Ratr0TreeSetNode *right;
    /** \brief parent node */
    struct Ratr0TreeSetNode *parent;

    /** \brief node color */
    UINT16 color;

    /** \brief payload */
    void *value;
};

/** \brief maximum numbers of nodes in a tree set */
#define MAX_TREESET_NODES (50)

/**
 * Representation of a tree set.
 */
struct Ratr0TreeSet {
    /*! \brief root node */
    struct Ratr0TreeSetNode *root;
    /** \brief NIL node */
    struct Ratr0TreeSetNode *NIL;
    /** \brief current number of elements in the set */
    UINT16 num_elements;
    /** \brief array of available nodes */
    struct Ratr0TreeSetNode nodes[MAX_TREESET_NODES];
    /** \brief index of the next available node in the nodes array */
    int next_node;
};

/**
 * The Ratr0TreeSets structure is the main interface for creating tree sets.
 */
struct Ratr0TreeSets {
    /** \brief Shuts down the treeset module. */
    void (*shutdown)(void);

    /**
     * \brief return a new tree set instance
     * @return pointer to an initialized TreeSet instance
     */
    struct Ratr0TreeSet *(*get_tree_set)(void);
};

/**
 * Initialize the tree sets module.
 *
 * @param engine the Ratr0Engine object
 * @return a pointer to the Ratr0TreeSets object, a singleton
 */
extern struct Ratr0TreeSets *ratr0_init_tree_sets(Ratr0Engine *engine);

//
// We only need 3 operations: insert, iteration and empty the entire thing.
//

/**
 * Insert an element into a tree set.
 *
 * @param set the set instance to insert into
 * @param value the pointer to the value to insert
 * @param lt a comparison function that returns TRUE if an element is less than another
          according to this set's ordering
 * @param eq an equality function that returns TRUE if an element is equal to another
 *        according to this set's equality rules
 * @return TRUE if element was inserted, FALSE if it could not be inserted
 */
extern BOOL ratr0_tree_set_insert(struct Ratr0TreeSet *set, void *value,
                            BOOL (*lt)(void *, void *),
                            BOOL (*eq)(void *, void *));

/**
 * Iterate over a tree set's elements. The tree will be traversed in inorder traversal.
 *
 * @param set the set to iterate over
 * @param process_node a process function that will be called in each iteration
 * @param user_data optional user data that is supplied to every call of process_node()
 */
extern void ratr0_tree_set_iterate(struct Ratr0TreeSet *set,
                             void (*process_node)(struct Ratr0TreeSetNode *, void *),
                             void *user_data);

/**
 * Removes all elements from a tree set.
 *
 * @param set the tree set to clear
 */
extern void ratr0_tree_set_clear(struct Ratr0TreeSet *set);
#endif /* __RATR0_TREESET_H__ */
