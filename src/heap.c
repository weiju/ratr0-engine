#include <ratr0/data_types.h>
#include <ratr0/heap.h>

/**
 * Pointer based priority queue.
 */
#define HEAP_PARENT(i) ((i - 1) / 2)
#define HEAP_LEFT(i) (i * 2 + 1)
#define HEAP_RIGHT(i) (i * 2 + 2)

static void swap(void **array, int a, int b)
{
    void *tmp;
    tmp = array[b];
    array[b] = array[a];
    array[a] = tmp;
}

static void heapify(void **array, int heap_size, int i, int (*gt)(void *, void *))
{
    int l = HEAP_LEFT(i), r = HEAP_RIGHT(i), largest;
    if (l < heap_size && gt(array[l], array[i])) {
        largest = l;
    } else {
        largest = i;
    }
    if (r < heap_size && gt(array[r], array[largest])) {
        largest = r;
    }
    if (largest != i) {
        swap(array, i, largest);
        heapify(array, heap_size, largest, gt);
    }
}

/**
 */
int ratr0_heap_insert(void *array[], int heap_size, void *key, int (*lt)(void *, void *))
{
    int i = heap_size;
    while (i > 0 && lt(array[HEAP_PARENT(i)], key)) {
        array[i] = array[HEAP_PARENT(i)];
        i = HEAP_PARENT(i);
    }
    array[i] = key;
    return heap_size + 1;
}

void *ratr0_heap_extract_max(void **array, int *heap_size, int (*gt)(void *, void *))
{
    int hs = *heap_size;
    if (hs == 0) {
        return NULL;
    }
    void *max = array[0];
    array[0] = array[hs - 1];
    hs--;
    heapify(array, hs, 0, gt);
    *heap_size = hs; // update heap size
    return max;
}

/*
typedef struct _coord { int x, y; } Coord;

int coord_greater(Coord *c1, Coord *c2) {
    return c1->y > c2->y || (c1->y == c2->y && c1->x > c2->y);
}

int coord_less(Coord *c1, Coord *c2) {
    return c1->y < c2->y || (c1->y == c2->y && c1->x < c2->y);
}


void print_array(Coord *array[], int heap_size)
{
    for (int i = 0; i < heap_size; i++) {
        printf("array[%d] = (%d, %d)\n", i, array[i]->x, array[i]->y);
    }
}

int main(int argc, char **argv)
{
    Coord pool[20];
    Coord *array[10];
    Coord c1 = { 3,  1};
    Coord c2 = { 5,  1};
    Coord c3 = { 1,  2};
    Coord c4 = { 7,  3};
    Coord c5 = { 11,  2};
    pool[0] = c1;
    pool[1] = c2;
    pool[2] = c3;
    pool[3] = c4;
    pool[4] = c5;

    int heap_size = 0;
    for (int i = 0; i < 5; i++) {
        heap_size = heap_insert((void **) array, heap_size, &pool[i],
                                (int (*)(void *, void *)) &coord_less);
    }
    printf("# heap size: %d\n", heap_size);
    print_array(array, heap_size);

    while (heap_size > 0) {
        Coord *max = heap_extract_max((void **) array, heap_size,
                                      (int (*)(void *, void *)) &coord_greater);
        printf("Extract: (%d, %d)\n", max->x, max->y);
        heap_size -= 1;
    }
    return 1;
}
*/
