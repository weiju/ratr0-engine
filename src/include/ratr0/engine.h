#pragma once
#ifndef __RATR0_ENGINE_H__
#define __RATR0_ENGINE_H__
/**
 * Top level module. This is the main interface to the user and the
 * management system for the subsystems.
 */
/**
 * Startup the engine and all its subsystems.
 */
extern void ratr0_engine_startup(void);

/**
 * Shutdown the engine and all its subsystems.
 */
extern void ratr0_engine_shutdown(void);

#endif /* __RATR0_ENGINE_H__ */
