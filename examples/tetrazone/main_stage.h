/** @file main_stage.h
 *
 * Tetra Zone main stage.
 */
#pragma once
#ifndef __MAIN_STAGE_H__
#define __MAIN_STAGE_H__

#include <ratr0/stages.h>
#include <ratr0/engine.h>

/**
 * Sets up the main stage object.
 *
 * @param engine pointer to Ratr0Engine instance
 * @return pointer to stage instance
 */
struct Ratr0Stage *setup_main_stage(Ratr0Engine *engine);

#endif // __MAIN_STAGE_H__
