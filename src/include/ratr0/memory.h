#pragma once
#ifndef __RATR0_MEMORY_H__
#define __RATR0_MEMORY_H__

#include <ratr0/data_types.h>

/*
 * RATR0 Engine Memory allocation subsystem. This is a simple abstraction
 * layer for allocating large blocks of memory that are used by different
 * subsystems of RATR0.
 * Subsystems should only allocate memory through this service, so memory
 * leaks are avoided. The general idea is that the subsystems allocate
 * larger blocks and manage specific object themselves.
 */
/**
 * Type of memory to reserve. Some systems (e.g.) will require special memory
 * to do certain operations.
 */
typedef enum { RATR0_MEM_DEFAULT, RATR0_MEM_CHIP } Ratr0MemoryType;

/**
 * This is the handle to the memory block. Memory access goes through handle.
 */
typedef INT32 Ratr0MemHandle;

struct Ratr0MemoryConfig {
    UINT32 general_pool_size;  // pool size in bytes
    UINT32 general_table_size; // num entries in table

    UINT32 chip_pool_size;   // pool size in bytes
    UINT32 chip_table_size;  // num entries in table
};

/*
 * The service interface is used to access the functions of the memory subsystem.
 */
struct Ratr0MemorySystem {
    Ratr0MemHandle (*allocate_block)(Ratr0MemoryType mem_type, UINT32 size);
    void (*free_block)(Ratr0MemHandle handle);
    void *(*block_address)(Ratr0MemHandle handle);
    void (*shutdown)(void);
};

/**
 * Start up the memory subsystem.
 */
extern struct Ratr0MemorySystem *ratr0_memory_startup(struct Ratr0MemoryConfig *);

#endif /* __RATR0_MEMORY_H__ */
