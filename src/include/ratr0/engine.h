#pragma once
#ifndef __RATR0_ENGINE_H__
#define __RATR0_ENGINE_H__
/**
 * Top level module. This is the main interface to the user and the
 * management system for the subsystems.
 */
typedef struct {
    struct Ratr0MemorySystem *memory_system;
    struct Ratr0DisplaySystem *display_system;
    void (*shutdown)();
} Ratr0Engine;

/**
 * Startup the engine and all its subsystems.
 */
extern Ratr0Engine *ratr0_engine_startup(void);

#endif /* __RATR0_ENGINE_H__ */
