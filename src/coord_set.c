#include <stdio.h>

#include <ratr0/data_types.h>
#include <ratr0/engine.h>
#include <ratr0/memory.h>
#include <ratr0/coord_set.h>

/**
 * Red black tree set implementation.
 * RB trees are binary search trees with the following properties:
 *   - every node is either red or black
 *   - every leaf node (null) is black
 *   - if a node is red, both its children are black
 *   - every simple path from a node to a descendant leaf contains the
 *     same number of black nodes
 *   - Insert, delete and search are O(log n)
 *
 * Implementation based on Cormen/Leiserson/Rivest "Introduction to Algorithms"
 *
 * with some double checking from:
 *
 * https://www.codesdope.com/course/data-structures-red-black-trees-insertion/
 */

static struct CoordSets coord_sets;
static struct CoordSet coord_set;  // just a singleton right now
static Ratr0Engine *engine;

struct Coord NIL_NODE, *NIL;

// TODO: Replace with allocation or something
// The maximum dirty 16x16 rectangles for a 320x256 screen
// is 320. I suspect there will be much less than 100 needed
// make this a sensitive value, since each node in this set is 22 bytes
#define MAX_NODES (40)
static struct Coord coords[MAX_NODES];
static int next_coord = 0;

void _coord_sets_shutdown(void) { }
struct CoordSet *_get_coord_set(void);

struct CoordSets *ratr0_startup_coord_sets(Ratr0Engine *eng)
{
    engine = eng;
    coord_sets.shutdown = &_coord_sets_shutdown;
    coord_sets.get_coord_set = &_get_coord_set;

    // initialize the singleton set
    NIL_NODE.color = RBT_BLACK;
    NIL_NODE.parent = NIL_NODE.left = NIL_NODE.right = NULL;
    NIL = &NIL_NODE;
    coord_set.root = NIL;
    coord_set.num_elements = 0;

    return &coord_sets;
}

struct CoordSet *_get_coord_set(void) { return &coord_set; }

/**
 * Red-Black Tree operations.
 */
void _rbt_left_rotate(struct CoordSet *set, struct Coord *x)
{
    struct Coord *y = x->right;
    x->right = y->left;
    if (y->left != NIL)
        y->left->parent = x;
    y->parent = x->parent;

    if (x->parent == NIL) {
        set->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void _rbt_right_rotate(struct CoordSet *set, struct Coord *x)
{
    struct Coord *y = x->left;
    x->left = y->right;
    if (y->right != NIL)
        y->right->parent = x;
    y->parent = x->parent;

    if (x->parent == NIL) {
        set->root = y;
    } else if (x == x->parent->right) {
        x->parent->right = y;
    } else {
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

BOOL coord_lt(struct Coord *a, struct Coord *b)
{
    return a->y < b->y || a->y == b->y && a->x < b->y;
}

BOOL coord_eq(struct Coord *a, struct Coord *b)
{
    return a->y == b->y && a->x == b->x;
}

/*
 * Only insert if element did not exist, if it exists, return FALSE
 */
BOOL _tree_insert(struct CoordSet *set, struct Coord *z)
{
    struct Coord *y = NIL, *x = set->root;

    while (x != NIL) {
        y = x;
        if (coord_eq(z, x)) return FALSE;  // FOUND IT !!!
        else if (coord_lt(z, x)) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    // not found -> actually insert that thing
    z->parent = y;
    if (y == NIL) {
        set->root = z;
    } else if (coord_lt(z, y)) {
        y->left = z;
    } else {
        y->right = z;
    }
    return TRUE;
}

BOOL coord_set_insert(struct CoordSet *set, UINT16 x, UINT16 y)
{
    // Part 1: BST tree insertion
    struct Coord *node_x = &coords[next_coord++];
    node_x->x = x;
    node_x->y = y;
    node_x->left = node_x->right = NIL;
    node_x->parent = NULL;

    if (!_tree_insert(set, node_x)) return FALSE;
    set->num_elements++;
    // Part 2: Red-black tree balancing
    node_x->color = RBT_RED;

    while (node_x->parent->color == RBT_RED) {
        if (node_x->parent == node_x->parent->parent->left) {
            // parent is left child of its parent
            struct Coord *y = node_x->parent->parent->right;
            if (y->color == RBT_RED) {
                node_x->parent->color = RBT_BLACK;
                y->color = RBT_BLACK;
                node_x->parent->parent->color = RBT_RED;
                node_x = node_x->parent->parent;
            } else {
                if (node_x == node_x->parent->right) {
                    node_x = node_x->parent;
                    _rbt_left_rotate(set, node_x);
                }
                node_x->parent->color = RBT_BLACK;
                node_x->parent->parent->color = RBT_RED;
                _rbt_right_rotate(set, node_x->parent->parent);
            }
        } else {
            // parent is right child of its parent
            struct Coord *y = node_x->parent->parent->left;
            if (y->color == RBT_RED) {
                node_x->parent->color = RBT_BLACK;
                y->color = RBT_BLACK;
                node_x->parent->parent->color = RBT_RED;
                node_x = node_x->parent->parent;
            } else {
                if (node_x == node_x->parent->left) {
                    node_x = node_x->parent;
                    _rbt_right_rotate(set, node_x);
                }
                node_x->parent->color = RBT_BLACK;
                node_x->parent->parent->color = RBT_RED;
                _rbt_left_rotate(set, node_x->parent->parent);
            }
        }
    }
    set->root->color = RBT_BLACK;
    return TRUE;
}

/**
 * Inorder processing of the CoordSet.
 */
void _rbt_inorder(struct Coord *node, void (*process_coord)(struct Coord *, void *), void *user_data)
{
    if (node != NIL) {
        _rbt_inorder(node->left, process_coord, user_data);
        process_coord(node, user_data);
        _rbt_inorder(node->right, process_coord, user_data);
    }
}

void coord_set_iterate(struct CoordSet *set,
                       void (*process_coord)(struct Coord *, void *), void *user_data)
{
    _rbt_inorder(set->root, process_coord, user_data);
}

void coord_set_clear(struct CoordSet *set)
{
    set->root = NIL;
    set->num_elements = 0;

    // Free all coord elements by resetting the index
    next_coord = 0;
}
