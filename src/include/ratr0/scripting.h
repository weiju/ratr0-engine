#pragma once
#ifndef __RATR0_SCRIPTING_H__
#define __RATR0_SCRIPTING_H__
#include <ratr0/engine.h>

/* Scripting subsystem */
struct Ratr0ScriptingSystem {
    void (*shutdown)(void);
};

/**
 * Start up the scripting subsystem.
 */
extern struct Ratr0ScriptingSystem *ratr0_scripting_startup(Ratr0Engine *);


#endif /* __RATR0_SCRIPTING_H__ */
