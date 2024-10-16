#include <ratr0/datastructs/vector.h>

static Ratr0Engine *engine;
#define MAX_VECTORS (10)

static struct Ratr0Vector vectors[MAX_VECTORS];
static int num_vectors;

void ratr0_vector_startup(Ratr0Engine *eng)
{
    engine = eng;
    num_vectors = 0;
}

void ratr0_vector_shutdown(void)
{
    for (int i = 0; i < num_vectors; i++) {
        ratr0_memory_free_block(vectors[i].handle);
    }
}

struct Ratr0Vector *ratr0_new_vector(UINT16 initial_capacity, UINT16 resize_by)
{
    struct Ratr0Vector *result = &vectors[num_vectors++];
    result->handle = ratr0_memory_allocate_block(RATR0_MEM_DEFAULT,
                                                 initial_capacity * sizeof(void *));
    result->elements = (void **) ratr0_memory_block_address(result->handle);
    result->num_elements = 0;
    result->capacity = initial_capacity;
    result->resize_by = resize_by;
    return result;
}

void ratr0_vector_append(struct Ratr0Vector *vec, void *elem)
{
    if (vec->num_elements == vec->capacity) {
        // create a new, larger array, copy elements and
        UINT16 new_capacity = vec->capacity + vec->resize_by;
        Ratr0MemHandle new_handle = ratr0_memory_allocate_block(RATR0_MEM_DEFAULT,
                                                                new_capacity * sizeof(void *));
        void **new_elems = ratr0_memory_block_address(new_handle);
        for (int i = 0; i < vec->num_elements; i++) {
            new_elems[i] = vec->elements[i];
        }
        // Free the old system and assign the new one
        ratr0_memory_free_block(vec->handle);
        vec->handle = new_handle;
        vec->elements = new_elems;
        vec->capacity = new_capacity;
    }
    vec->elements[vec->num_elements++] = elem;
}

void ratr0_vector_clear(struct Ratr0Vector *vec)
{
    vec->num_elements = 0;
}
