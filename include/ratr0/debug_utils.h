#pragma once
#ifndef __RATR0_DEBUG_UTILS_H__
#define __RATR0_DEBUG_UTILS_H__
#include <stdio.h>

#ifdef DEBUG
#define PRINT_DEBUG(...) printf(__VA_ARGS__)
#else
#define PRINT_DEBUG(...)
#endif

#endif /* __RATR0_DEBUG_UTILS_H__ */
