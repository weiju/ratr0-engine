/** @file inv_main_stage.h
 *
 * Invaders main stage.
 */
#pragma once
#ifndef __INV_MAIN_STAGE_H__
#define __INV_MAIN_STAGE_H__

#include <ratr0/stages.h>
#include <ratr0/engine.h>

/**
 * Sets up the main stage object.
 *
 * @param engine pointer to Ratr0Engine instance
 * @return pointer to stage instance
 */
struct Ratr0Stage *setup_main_stage(Ratr0Engine *engine);

#endif /* __INV_MAIN_STAGE_H__ */
