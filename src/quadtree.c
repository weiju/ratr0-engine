#include <ratr0/quadtree.h>

static Ratr0Engine *engine;
#define MAX_QUADTREE_NODES (16)

struct Ratr0QuadTreeNode nodes[MAX_QUADTREE_NODES];
static int next_node = 0;

void ratr0_init_quadtrees(Ratr0Engine *eng)
{
    engine = eng;
    next_node = 0;
}

void ratr0_shutdown_quadtrees(void)
{
}

struct Ratr0QuadTreeNode *_new_quadtree_node(UINT16 x, UINT16 y, UINT16 width, UINT16 height)
{
    struct Ratr0QuadTreeNode *result = &nodes[next_node++];
    result->bounds.x = x;
    result->bounds.y = y;
    result->bounds.width = width;
    result->bounds.height = height;

    for (int i = 0; i < 4; i++) {
        result->quadrants[i] = NULL;
    }
    result->is_leaf = TRUE;
    result->num_elems = 0;
    return result;
}

struct Ratr0QuadTreeNode *ratr0_new_quad_tree(UINT16 x, UINT16 y, UINT16 width, UINT16 height)
{
    next_node = 0;
    return _new_quadtree_node(x, y, width, height);
}

void ratr0_quadtree_split_node(struct Ratr0QuadTreeNode *node)
{
    if (node->is_leaf) {
        node->is_leaf = FALSE;
        UINT16 qwidth = node->bounds.width / 2, qheight = node->bounds.height / 2;
        node->quadrants[0] = _new_quadtree_node(node->bounds.x, node->bounds.y,
                                                qwidth, qheight);
        node->quadrants[1] = _new_quadtree_node(node->bounds.x + qwidth, node->bounds.y,
                                                qwidth, qheight);
        node->quadrants[2] = _new_quadtree_node(node->bounds.x, node->bounds.y + qheight,
                                                qwidth, qheight);
        node->quadrants[3] = _new_quadtree_node(node->bounds.x + qwidth, node->bounds.y + qheight,
                                                qwidth, qheight);
    }
}

UINT8 ratr0_quadtree_quadrants(struct Ratr0QuadTreeNode *node, struct Ratr0BoundingBox *elem,
                               UINT8 results[4])
{
    int num_results = 0;
    for (int i = 0; i < 4; i++) {
        if (ratr0_bb_overlap(elem, &node->quadrants[i]->bounds)) {
            results[num_results++] = i;
        }
    }
    return num_results;
}

void ratr0_quadtree_clear(void)
{
    nodes[0].num_elems = 0;
    next_node = 1;
}

void ratr0_quadtree_insert(struct Ratr0QuadTreeNode *node, struct Ratr0BoundingBox *elem)
{
    if (node->is_leaf) {
        if (node->num_elems >= RATR0_QT_SPLIT_THRESH) {
            ratr0_quadtree_split_node(node);
            // copy child elements to quadrants, note that this is quite expensive
            for (int i = 0; i < node->num_elems; i++) {
                for (int q = 0; q < 4; q++) {
                    if (ratr0_bb_overlap(node->elems[i], &node->quadrants[q]->bounds)) {
                        node->quadrants[q]->elems[node->quadrants[q]->num_elems++] = node->elems[i];
                    }
                }
            }
            // and remove them from the node
            node->num_elems = 0;
            // finaly, insert the node
            ratr0_quadtree_insert(node, elem);
        } else {
            node->elems[node->num_elems++] = elem;
        }
    } else {
        // insert in to all of the quadrants the element is overlapping with
        UINT8 indexes[4], num_indexes;
        num_indexes = ratr0_quadtree_quadrants(node, elem, indexes);
        for (int i = 0; i < num_indexes; i++) {
            ratr0_quadtree_insert(node->quadrants[indexes[i]], elem);
        }
    }
}

BOOL ratr0_bb_overlap(struct Ratr0BoundingBox *r1, struct Ratr0BoundingBox *r2)
{
    return r1->x < (r2->x + r2->width) && (r1->x + r1->width) > r2->x &&
        r1->y < (r2->y + r2->height) && (r1->y + r1->height) > r2->y;
}

void ratr0_quadtree_overlapping(struct Ratr0QuadTreeNode *node,
                                struct Ratr0BoundingBox *elem,
                                struct Ratr0Vector *result)
{
    if (node->is_leaf) {
        // search elements
        for (int i = 0; i < node->num_elems; i++) {
            if (ratr0_bb_overlap(elem, node->elems[i])) {
                ratr0_vector_append(result, node->elems[i]);
            }
        }
    } else {
        // TODO: search the 4 children recursively
    }
}
