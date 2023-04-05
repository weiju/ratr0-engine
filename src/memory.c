#include <stdio.h>
#include <stdlib.h>

#include <ratr0/debug_utils.h>
#include <ratr0/memory.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[33mMEMORY\033[0m", __VA_ARGS__)

#ifdef AMIGA
#include <ratr0/amiga/memory.h>
#else

static void *general_mem_pool;
static INT32 general_pool_size;

static void **general_mem_table;
static INT32 general_table_size;
static INT32 first_free_table, first_free;
#endif

/*
 * The global memory service instance.
 */
struct Ratr0MemoryService Ratr0MemoryService;

// Forward declarations for the generic memory allocator
Ratr0MemHandle ratr0_memory_allocate_block(Ratr0MemoryType mem_type, UINT32 size);
void ratr0_memory_free_block(Ratr0MemHandle handle);
void *ratr0_memory_block_address(Ratr0MemHandle handle);


void ratr0_memory_startup(struct Ratr0MemoryConfig *config)
{
    PRINT_DEBUG("Start up...");

 #ifdef AMIGA
    ratr0_amiga_memory_startup(config);

    Ratr0MemoryService.allocate_block = &ratr0_amiga_memory_allocate_block;
    Ratr0MemoryService.free_block = &ratr0_amiga_memory_free_block;
    Ratr0MemoryService.block_address = &ratr0_amiga_memory_block_address;
 #else
    // Initialize the memory table
    general_table_size = config->general_table_size;
    general_pool_size = config->general_pool_size;
    general_mem_table = (void **) malloc(sizeof(void *) * general_table_size);
    general_mem_pool = (void *) malloc(sizeof(char) * general_pool_size);
    for (INT32 i = 0; i < general_table_size; i++) general_mem_table[i] = NULL;

    first_free = first_free_table = 0;

    Ratr0MemoryService.allocate_block = &ratr0_memory_allocate_block;
    Ratr0MemoryService.free_block = &ratr0_memory_free_block;
    Ratr0MemoryService.block_address = &ratr0_memory_block_address;
#endif

    PRINT_DEBUG("Startup finished.");
}

void ratr0_memory_shutdown(void)
{
    PRINT_DEBUG("Shutting down...");

#ifdef AMIGA
    ratr0_amiga_memory_shutdown();
#else
    // Free memory for the memory object table and the pool
    free(general_mem_table);
    free(general_mem_pool);
#endif
    PRINT_DEBUG("Shutdown finished.");
}

#ifndef AMIGA
Ratr0MemHandle ratr0_memory_allocate_block(Ratr0MemoryType mem_type, UINT32 num_bytes)
{
    Ratr0MemHandle result = first_free_table;
    void *mem_block = (void *) (general_mem_pool + first_free);
    general_mem_table[first_free_table++] = mem_block;
    first_free += num_bytes;  // increment the memory pool pointer
    return result;
}

void ratr0_memory_free_block(Ratr0MemHandle handle)
{
    // This is a no-op for now
}

void *ratr0_memory_block_address(Ratr0MemHandle handle)
{
    return general_mem_table[handle];
}

#endif
