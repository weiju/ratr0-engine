/** @file treeset.c */
#include <stdio.h>

#include <ratr0/data_types.h>
#include <ratr0/engine.h>
#include <ratr0/memory.h>
#include <ratr0/treeset.h>

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

static struct Ratr0TreeSets tree_sets;
#define MAX_INSTANCES (3)
static struct Ratr0TreeSet tree_set_instances[MAX_INSTANCES];  // just a singleton right now
static int next_instance;
static Ratr0Engine *engine;

void _tree_sets_shutdown(void) { }
struct Ratr0TreeSet *_get_tree_set(void);

struct Ratr0TreeSets *ratr0_init_tree_sets(Ratr0Engine *eng)
{
    engine = eng;
    tree_sets.shutdown = &_tree_sets_shutdown;
    tree_sets.get_tree_set = &_get_tree_set;
    next_instance = 0;
    return &tree_sets;
}

struct Ratr0TreeSet *_get_tree_set(void) {
    struct Ratr0TreeSet *result = &tree_set_instances[next_instance++];
    result->next_node = 0;
    // index zero will always be NIL
    struct Ratr0TreeSetNode *NIL = &result->nodes[result->next_node++];
    // initialize the singleton set
    NIL->color = RBT_BLACK;
    NIL->parent = NIL->left = NIL->right = NULL;

    result->root = NIL;
    result->NIL = NIL;
    result->num_elements = 0;
    return result;
}

/**
 * Red-Black Tree operations.
 */
void _rbt_left_rotate(struct Ratr0TreeSet *set, struct Ratr0TreeSetNode *x)
{
    struct Ratr0TreeSetNode *y = x->right;
    x->right = y->left;
    if (y->left != set->NIL)
        y->left->parent = x;
    y->parent = x->parent;

    if (x->parent == set->NIL) {
        set->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void _rbt_right_rotate(struct Ratr0TreeSet *set, struct Ratr0TreeSetNode *x)
{
    struct Ratr0TreeSetNode *y = x->left;
    x->left = y->right;
    if (y->right != set->NIL)
        y->right->parent = x;
    y->parent = x->parent;

    if (x->parent == set->NIL) {
        set->root = y;
    } else if (x == x->parent->right) {
        x->parent->right = y;
    } else {
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

/*
 * Only insert if element did not exist, if it exists, return FALSE
 */
BOOL _tree_insert(struct Ratr0TreeSet *set, struct Ratr0TreeSetNode *z,
                  BOOL (*_lt)(void *, void *),
                  BOOL (*_eq)(void *, void *))
{
    struct Ratr0TreeSetNode *y = set->NIL, *x = set->root;

    while (x != set->NIL) {
        y = x;
        if (_eq(z->value, x->value)) return FALSE;  // FOUND IT !!!
        else if (_lt(z->value, x->value)) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    // not found -> actually insert that thing
    z->parent = y;
    if (y == set->NIL) {
        set->root = z;
    } else if (_lt(z->value, y->value)) {
        y->left = z;
    } else {
        y->right = z;
    }
    return TRUE;
}

BOOL ratr0_tree_set_insert(struct Ratr0TreeSet *set, void *value,
                     BOOL (*_lt)(void *, void *),
                     BOOL (*_eq)(void *, void *))
{
    // Part 1: BST tree insertion
    struct Ratr0TreeSetNode *node_x = &set->nodes[set->next_node++];
    node_x->value = value;
    node_x->left = node_x->right = set->NIL;
    node_x->parent = NULL;

    if (!_tree_insert(set, node_x, _lt, _eq)) return FALSE;
    set->num_elements++;
    // Part 2: Red-black tree balancing
    node_x->color = RBT_RED;

    while (node_x->parent->color == RBT_RED) {
        if (node_x->parent == node_x->parent->parent->left) {
            // parent is left child of its parent
            struct Ratr0TreeSetNode *y = node_x->parent->parent->right;
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
            struct Ratr0TreeSetNode *y = node_x->parent->parent->left;
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
 * Inorder processing of the Ratr0TreeSet.
 */
void _rbt_inorder(struct Ratr0TreeSet *set, struct Ratr0TreeSetNode *node,
                  void (*process_node)(struct Ratr0TreeSetNode *, void *),
                  void *user_data)
{
    if (node != set->NIL) {
        _rbt_inorder(set, node->left, process_node, user_data);
        process_node(node, user_data);
        _rbt_inorder(set, node->right, process_node, user_data);
    }
}

void ratr0_tree_set_iterate(struct Ratr0TreeSet *set,
                       void (*process_node)(struct Ratr0TreeSetNode *, void *), void *user_data)
{
    _rbt_inorder(set, set->root, process_node, user_data);
}

void ratr0_tree_set_clear(struct Ratr0TreeSet *set)
{
    set->root = set->NIL;
    set->num_elements = 0;

    // Free all node elements by resetting the index to 1, remember index 0 is NIL
    set->next_node = 1;
}
