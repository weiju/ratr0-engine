#include <ratr0/bitset.h>
#include <ratr0/coord_set.h>
#include <ratr0/engine.h>
#include <ratr0/memory.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static Ratr0Engine mock_engine;
static struct Ratr0MemorySystem memsys;
static void *mock_mem;

Ratr0MemHandle mock_allocate_block(Ratr0MemoryType mem_type, UINT32 size)
{
    mock_mem = malloc(size);
    return 0;
}

void mock_free_block(Ratr0MemHandle handle) {
    if (mock_mem) {
        free(mock_mem);
        mock_mem = NULL;
    }
}
void *mock_block_address(Ratr0MemHandle handle) { return mock_mem; }

struct CoordSets *coord_sets;

void init_coord_sets(void)
{
    mock_mem = NULL;
    memsys.allocate_block = &mock_allocate_block;
    memsys.free_block = &mock_free_block;
    memsys.block_address = &mock_block_address;
    mock_engine.memory_system = &memsys;
    coord_sets = ratr0_startup_coord_sets(&mock_engine);
}

void shutdown_coord_sets(void) {
    if (mock_mem) {
        free(mock_mem);
        mock_mem = NULL;
    }
    coord_sets->shutdown();
}

void print_index(UINT16 index)
{
    printf("BITSET INDEX: %u\n", index);
}


struct timespec diff(struct timespec start, struct timespec end)
{
    struct timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}

int bscounter = 0;
int rbcounter = 0;

void bitset_fun(UINT16 index)
{
    bscounter++;
}

void coordset_fun(struct Coord *c, void *user_data)
{
    rbcounter++;
}

void insert_rects(UINT32 *bitset_arr, struct CoordSet *coord_set)
{
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time);
    // Insertion into bitset
    for (int k = 0; k < 100; k++) {
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 20; j++) {
                ratr0_bitset_insert(bitset_arr, 10, i * 20 + j);
            }
        }
        ratr0_bitset_clear(bitset_arr, 10);
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time);
    struct timespec elapsed = diff(start_time, end_time);
    printf("BITSET elapsed: %ld sec %ld nsec\n", elapsed.tv_sec, elapsed.tv_nsec);

    // Insertion into RB set
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time);
    for (int k = 0; k < 100; k++) {
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 20; j++) {
                coord_set_insert(coord_set, j, i);
            }
        }
        coord_set_clear(coord_set);
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time);
    elapsed = diff(start_time, end_time);
    printf("RBTREE elapsed: %ld sec %ld nsec\n", elapsed.tv_sec, elapsed.tv_nsec);

    // Now iterate over 100 elements
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 20; j++) {
            ratr0_bitset_insert(bitset_arr, 10, i * 20 + j);
            coord_set_insert(coord_set, j, i);
        }
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time);
    for (int k = 0; k < 1000; k++) {
        ratr0_bitset_iterate(bitset_arr, 10, &bitset_fun);
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time);
    elapsed = diff(start_time, end_time);
    printf("BITSET ITERATE elapsed: %ld sec %ld nsec\n", elapsed.tv_sec, elapsed.tv_nsec);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time);
    for (int k = 0; k < 1000; k++) {
        coord_set_iterate(coord_set, &coordset_fun, NULL);
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time);
    elapsed = diff(start_time, end_time);
    printf("RBTREE ITERATE elapsed: %ld sec %ld nsec\n", elapsed.tv_sec, elapsed.tv_nsec);
}

int main(int argc, char **argv)
{
    UINT32 bitset_arr[10];  // 320 elements
    init_coord_sets();
    struct CoordSet *coord_set = coord_sets->get_coord_set();
    ratr0_bitset_clear(bitset_arr, 10);

    insert_rects(bitset_arr, coord_set);

    shutdown_coord_sets();
    return 0;
}
