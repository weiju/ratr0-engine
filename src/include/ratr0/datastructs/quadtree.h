/** @file quadtree.h
 *
 * Quadtree implementation for spatial partition. Supports efficient
 * collision/overlap detection.
 * We need to be able to construct and clear a quadtree within a single frame.
 * Therefore we need operations of
 *   - insert/lookup in O(log(n))
 *   - clear in O(1)
 */
#pragma once
#ifndef __RATR0_QUADTREE_H__
#define __RATR0_QUADTREE_H__

#include <ratr0/data_types.h>
#include <ratr0/engine.h>
#include <ratr0/datastructs/vector.h>

/** \brief maximum number of elements that can be stored in a quad tree left node */
#define RATR0_MAX_QUADTREE_ELEMS (10)

/** \brief element count threshold before splitting a leaf */
#define RATR0_QT_SPLIT_THRESH (6)

/**
 * Representation of a quad tree node.
 */
struct Ratr0QuadTreeNode {
    /** \brief bounding box of this node */
    struct Ratr0BoundingBox bounds;

    /** \brief quadrant child nodes */
    struct Ratr0QuadTreeNode *quadrants[4];

    /** \brief indicates if this node is a leaf */
    BOOL is_leaf;
    /** \brief if leaf node, elements are store here */
    struct Ratr0BoundingBox *elems[RATR0_MAX_QUADTREE_ELEMS];
    /** \brief number of actual elements stored */
    UINT8 num_elems;
};

/**
 * Initialize the quadtree module.
 *
 * @param engine Ratr0Engine instance
 */
extern void ratr0_init_quadtrees(Ratr0Engine *engine);

/**
 * Shutdown the quadtree module.
 */
extern void ratr0_shutdown_quadtrees(void);

/**
 * Create a new quad tree. There can only be one quad tree in the system.
 *
 * @param x x-coordinate of the bounding box
 * @param y y-coordinate of the bounding box
 * @param width width of the bounding box
 * @param height height of the bounding box
 * @return root node of the tree
 */
extern struct Ratr0QuadTreeNode *ratr0_new_quad_tree(UINT16 x, UINT16 y, UINT16 width, UINT16 height);

/**
 * Clears the current quad tree, preserving the old node dimensions.
 */
extern void ratr0_quadtree_clear(void);

/**
 * Insert an element into a quadtree.
 *
 * @param node the node to insert the element into
 * @param elem the element to insert
 */
extern void ratr0_quadtree_insert(struct Ratr0QuadTreeNode *node, struct Ratr0BoundingBox *elem);

/**
 * Bounding Box intersection test.
 *
 * @param r1 first bounding box
 * @param r2 second bounding box
 * @return TRUE if both boxes overlap, FALSE otherwise
 */
extern BOOL ratr0_bb_overlap(struct Ratr0BoundingBox *r1, struct Ratr0BoundingBox *r2);

/**
 * Retrieve all objects in the tree that overlap with elem.
 *
 * @param node the node to search
 * @param elem the element to match against
 * @param result the vector to store the results
 */
extern void ratr0_quadtree_overlapping(struct Ratr0QuadTreeNode *node,
                                       struct Ratr0BoundingBox *elem,
                                       struct Ratr0Vector *result);


/**
 * Given non-leaf node and a bounding box, return all quadrant indexes
 * the bounding box can be placed in. Can return 1, 2 or 4 results, depending on
 * how the bounding box is positioned.
 *
 * @param node the non-leaf node to query
 * @param elem the bounding box element to place
 * @param results the results array
 * @return number of results
 */
extern UINT8 ratr0_quadtree_quadrants(struct Ratr0QuadTreeNode *node,
                                     struct Ratr0BoundingBox *elem,
                                     UINT8 results[4]);

/**
 * Splits a leaf node, after this operation node will not be a leaf anymore
 * and have 4 quadrant children.
 *
 * @param node the node to split
 */
void ratr0_quadtree_split_node(struct Ratr0QuadTreeNode *node);

#endif /* __RATR0_QUADTREE_H__ */
