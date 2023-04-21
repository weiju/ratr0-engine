#pragma once
#ifndef __RATR0_HEAP_H__
#define __RATR0_HEAP_H__

extern int ratr0_heap_insert(void *array[], int heap_size, void *key, int (*lt)(void *, void *));
extern void *ratr0_heap_extract_max(void **array, int *heap_size, int (*gt)(void *, void *));

#endif /* __RATR0_HEAP_H__ */
