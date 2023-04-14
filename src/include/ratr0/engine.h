#pragma once
#ifndef __RATR0_ENGINE_H__
#define __RATR0_ENGINE_H__

/**
 * Top level module. This is the main interface to the user and the
 * management system for the subsystems.
 */
typedef struct {
    struct Ratr0MemorySystem *memory_system;
    struct Ratr0EventSystem *event_system;
    struct Ratr0TimerSystem *timer_system;
    struct Ratr0InputSystem *input_system;
    struct Ratr0DisplaySystem *display_system;
    struct Ratr0AudioSystem *audio_system;
    struct Ratr0ResourceSystem *resource_system;
    struct Ratr0PhysicsSystem *physics_system;
    struct Ratr0SceneSystem *scene_system;
    struct Ratr0ScriptingSystem *scripting_system;

    void (*shutdown)(void);
    void (*game_loop)(void);

    /**
     * This method closes the entire system down, so we can shut down
     * our game here.
     */
    void (*exit)(void);
} Ratr0Engine;

/**
 * Startup the engine and all its subsystems.
 */
extern Ratr0Engine *ratr0_engine_startup(void);

#endif /* __RATR0_ENGINE_H__ */
