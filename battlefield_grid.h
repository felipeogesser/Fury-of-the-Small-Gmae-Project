#ifndef BATTLEFIELD_GRID_H
#define BATTLEFIELD_GRID_H

#include <stddef.h>

extern size_t armies_total_memory_size;

void init_grids(void);

void free_grid_memory(void);

#endif