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
