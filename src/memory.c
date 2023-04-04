#include <stdio.h>
#include <stdlib.h>

#include <ratr0/debug_utils.h>
#include <ratr0/memory.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[33mMEMORY\033[0m", __VA_ARGS__)

const int SYSTEM_MEM_SIZE = 10;
static void **system_mem_table;
static INT32 first_free_entry;

void ratr0_memory_startup(void)
{
    PRINT_DEBUG("Start up...");

    // Initialize the memory table
    INT32 memtable_size = SYSTEM_MEM_SIZE;
    system_mem_table = (void **) malloc(sizeof(void *) * memtable_size);
    first_free_entry = 0;
    for (INT32 i = 0; i < memtable_size; i++) system_mem_table[i] = NULL;

    PRINT_DEBUG("Startup finished.");
}

void ratr0_memory_shutdown(void)
{
    PRINT_DEBUG("Shutting down...");

    // Free all entries in the table. Currently, we do not free any blocks
    // during runtime, so we just iterate from 0 to first_free_entry
    for (INT32 i = 0; i < first_free_entry; i++) {
        if (system_mem_table[i]) {
            PRINT_DEBUG("Freeing memory block at index: %d", i);
            free(system_mem_table[i]);
            system_mem_table[i] = NULL;
        }
    }
    free(system_mem_table);
    PRINT_DEBUG("Shutdown finished.");
}

Ratr0MemHandle ratr0_memory_allocate_block(Ratr0MemoryType mem_type, INT32 num_bytes)
{
    Ratr0MemHandle result = first_free_entry;
    void *mem_block = malloc(sizeof(char) * num_bytes);
    system_mem_table[first_free_entry++] = mem_block;
    return result;
}

void ratr0_memory_free_block(Ratr0MemHandle handle)
{
    // This is a no-op for now
}
