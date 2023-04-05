#pragma once
#ifndef __RATR0_MEMORY_H__
#define __RATR0_MEMORY_H__

#include <ratr0/data_types.h>

/*
 * RATR0 Engine Memory allocation subsystem. This is a simple abstraction
 * layer for allocating large blocks of memory that are used by different
 * subsystems of RATR0.
 * Subsystems should only allocate memory through this service, so memory
 * leaks are avoided.
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


/*
 * The service interface is used to access the functions of the memory subsystem.
 */
struct Ratr0MemoryService {
    Ratr0MemHandle (*allocate_block)(Ratr0MemoryType mem_type, UINT32 size);
    void (*free_block)(Ratr0MemHandle handle);
};
extern struct Ratr0MemoryService Ratr0MemoryService;

/**
 * Start up the memory subsystem.
 */
extern void ratr0_memory_startup(void);

/**
 * Shut down the memory subsystem.
 */
extern void ratr0_memory_shutdown(void);


#endif /* __RATR0_MEMORY_H__ */
