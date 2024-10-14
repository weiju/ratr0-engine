/** @file memory.h
 *
 * RATR0 Engine Memory allocation subsystem. This is a simple abstraction
 * layer for allocating large blocks of memory that are used by different
 * subsystems of RATR0.
 * Subsystems should only allocate memory through this service, so memory
 * leaks are avoided. The general idea is that the subsystems allocate
 * larger blocks and manage specific object themselves.
 */
#pragma once
#ifndef __RATR0_MEMORY_H__
#define __RATR0_MEMORY_H__

#include <ratr0/data_types.h>
#include <ratr0/engine.h>

/**
 * \brief Type of memory to reserve. Some systems (e.g.) will require special memory
 * to do certain operations.
 */
typedef enum { RATR0_MEM_DEFAULT, RATR0_MEM_CHIP } Ratr0MemoryType;

/**
 * \brief handle to a memory block. Memory access goes through handle.
 */
typedef INT32 Ratr0MemHandle;

/**
 * Pre-allocate memory at the start of the game. This ensures we never explicitly
 * allocate or deallocate memory outside of the engine.
 */
struct Ratr0MemoryConfig {
    /** \brief pool size in bytes */
    UINT32 general_pool_size;
    /** \brief num entries in table */
    UINT32 general_table_size;

    /** \brief pool size in bytes */
    UINT32 chip_pool_size;
    /** \brief num entries in table */
    UINT32 chip_table_size;
};

/**
 * The service interface is used to access the functions of the memory subsystem.
 */
struct Ratr0MemorySystem {

    /**
     * Shuts down the memory subsystem.
     */
    void (*shutdown)(void);
};

/**
 * Start up the memory subsystem.
 *
 * @param engine pointer to Ratr0Engine instance
 * @param config configuration object
 */
extern struct Ratr0MemorySystem *ratr0_memory_startup(Ratr0Engine *engine,
                                                      struct Ratr0MemoryConfig *config);

/**
 * Allocates a memory block.
 *
 * @param mem_type memory type
 * @param size size of the memory block
 */
extern Ratr0MemHandle ratr0_memory_allocate_block(Ratr0MemoryType mem_type,
                                                  UINT32 size);

/**
 * Free the specified memory block.
 *
 * @param handle handle to the memory block that should be freed
 */
extern void ratr0_memory_free_block(Ratr0MemHandle handle);

/**
 * Returns the physical memory address given a handle.
 *
 * @param handle
 * @return physical memory address
 */
extern void *ratr0_memory_block_address(Ratr0MemHandle handle);

#endif /* __RATR0_MEMORY_H__ */
