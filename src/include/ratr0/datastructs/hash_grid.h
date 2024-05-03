/** @file hash_grid.h - Hash grid implementation for efficient collision detection
 */
#pragma once
#ifndef __HASH_GRID_H__
#define __HASH_GRID_H__
#include <ratr0/data_types.h>

/**
 * Implementation of a spatial hash. In essence, this is a grid with a fixed cell size
 * and each cell is a bucket that contains all objects that are contained within the
 * cell.
 */
#define MAX_HASHGRID_BUCKET_ELEMENTS (30)
struct Ratr0HashGridCell {
    void *bucket[MAX_HASHGRID_BUCKET_ELEMENTS];
    UINT16 num_elements;
};

struct Ratr0HashGrid {
    UINT16 cell_size;
    UINT16 num_rows;
    UINT16 num_columns;
    struct Ratr0HashGridCell **grid;
};

#endif /* __HASH_GRID_H__ */
