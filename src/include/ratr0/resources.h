#pragma once
#ifndef __RATR0_RESOURCES_H__
#define __RATR0_RESOURCES_H__

/* Resources subsystem */
struct Ratr0ResourceSystem {
    void (*shutdown)(void);
};

/**
 * Start up the resources subsystem.
 */
extern struct Ratr0ResourceSystem *ratr0_resources_startup(void);

#endif /* __RATR0_RESOURCES_H__ */
