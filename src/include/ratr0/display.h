#pragma once
#ifndef __RATR0_DISPLAY_H__
#define __RATR0_DISPLAY_H__
#include <ratr0/data_types.h>

/* Display subsystem */
struct Ratr0DisplayInfo {
    UINT16 width, height;
    UINT8 depth;
};

/**
 * Start up the display subsystem.
 */
extern void ratr0_display_startup(struct Ratr0DisplayInfo *);

/**
 * Shut down the display subsystem.
 */
extern void ratr0_display_shutdown(void);


#endif /* __RATR0_DISPLAY_H__ */
