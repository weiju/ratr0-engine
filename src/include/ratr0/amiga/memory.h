#pragma once
#ifndef __RATR0_AMIGA_MEMORY_H__
#define __RATR0_AMIGA_MEMORY_H__
#include <ratr0/data_types.h>
#include <ratr0/memory.h>

/* Amiga Memory subsystem */

/**
 * Start up the memory subsystem.
 */
extern void ratr0_amiga_memory_startup(struct Ratr0MemoryConfig *);

/**
 * Shut down the memory subsystem.
 */
extern void ratr0_amiga_memory_shutdown(void);

extern Ratr0MemHandle ratr0_amiga_memory_allocate_block(Ratr0MemoryType mem_type, UINT32 size);
extern void ratr0_amiga_memory_free_block(Ratr0MemHandle handle);
extern void *ratr0_amiga_memory_block_address(Ratr0MemHandle handle);


#endif /* __RATR0_AMIGA_MEMORY_H__ */
