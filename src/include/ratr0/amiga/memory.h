/** @file memory.h
 *
 * Amiga Memory subsystem
 */
#pragma once
#ifndef __RATR0_AMIGA_MEMORY_H__
#define __RATR0_AMIGA_MEMORY_H__
#include <ratr0/data_types.h>
#include <ratr0/memory.h>

/**
 * Start up the memory subsystem.
 *
 * @param config configuration object
 */
extern void ratr0_amiga_memory_startup(struct Ratr0MemoryConfig *config);

/**
 * Shut down the memory subsystem.
 */
extern void ratr0_amiga_memory_shutdown(void);

/**
 * Allocates a memory block.
 *
 * @param mem_type memory type
 * @param size block size
 * @return handle to the allocated memory
 */
extern Ratr0MemHandle ratr0_amiga_memory_allocate_block(Ratr0MemoryType mem_type, UINT32 size);

/**
 * Frees a memory block.
 *
 * @param handle handle to the memory block
 */
extern void ratr0_amiga_memory_free_block(Ratr0MemHandle handle);

/**
 * Retrieve the physical memory address for a given handle.
 *
 * @param handle memory handle
 * @return pointer to memory block
 */
extern void *ratr0_amiga_memory_block_address(Ratr0MemHandle handle);


#endif /* __RATR0_AMIGA_MEMORY_H__ */
