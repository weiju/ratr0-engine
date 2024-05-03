/** @file engine.h
 *
 * Top level engine module. This is the main interface to the user and the
 * management system for the subsystems.
 */
#pragma once
#ifndef __RATR0_ENGINE_H__
#define __RATR0_ENGINE_H__

struct Ratr0MemorySystem;
struct Ratr0MemoryConfig;
struct Ratr0DisplayInfo;

/**
 * Engine interface.
 */
typedef struct {
    /** \brief memory subsystem */
    struct Ratr0MemorySystem *memory_system;
    /** \brief event subsystem */
    //struct Ratr0EventSystem *event_system;
    /** \brief timer subsystem */
    struct Ratr0TimerSystem *timer_system;
    /** \brief input subsystem */
    struct Ratr0InputSystem *input_system;
    /** \brief rendering subsystem */
    struct Ratr0RenderingSystem *rendering_system;
    /** \brief audio subsystem */
    struct Ratr0AudioSystem *audio_system;
    /** \brief resource subsystem */
    struct Ratr0ResourceSystem *resource_system;
    /** \brief scene subsystem */
    struct Ratr0ScenesSystem *scenes_system;

    /**
     * Shuts down the engine.
     */
    void (*shutdown)(void);

    /**
     * Game loop function.
     */
    void (*game_loop)(void);

    /**
     * This method closes the entire system down, so we can shut down
     * our game here.
     */
    void (*exit)(void);
} Ratr0Engine;

/**
 * Startup the engine and all its subsystems.
 *
 * @param memory_config memory configuration
 * @param display_info display configuration
 * @return pointer to the initialized engine system
 */
extern Ratr0Engine *ratr0_engine_startup(struct Ratr0MemoryConfig *memory_config,
                                         struct Ratr0DisplayInfo *display_info);

/**
 * Call this to shutdown the engine.
 */
extern void ratr0_engine_exit(void);

#endif /* __RATR0_ENGINE_H__ */
