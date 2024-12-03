/** @file debug_utils.h */
#pragma once
#ifndef __RATR0_DEBUG_UTILS_H__
#define __RATR0_DEBUG_UTILS_H__
#include <stdio.h>
#include <string.h>
#include <time.h>

/**
 * Standard debug print function, prints the tag and the log time.
 */
#ifdef DEBUG
#define PRINT_DEBUG_TAG(tag, ...) { time_t now; time(&now); fprintf(debug_fp, "%s [%s] - ", tag, strtok(ctime(&now), "\n")); fprintf(debug_fp, __VA_ARGS__); fputs("\n", debug_fp);}
#else
#define PRINT_DEBUG_TAG(tag, ...)
#endif

#endif /* __RATR0_DEBUG_UTILS_H__ */
