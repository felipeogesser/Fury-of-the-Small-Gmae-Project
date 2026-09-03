#ifndef LOAD_ARMIES_H
#define LOAD_ARMIES_H

#include <stddef.h>

void init_armies_memory_arena(unsigned int armies_count, unsigned int battalion_count, unsigned int battalion_size);
void load_armies_into_arena(unsigned int armies_count, unsigned int battalion_count, unsigned int battalion_size);
void free_army_memory(void);

extern size_t armies_total_memory_size;

#endif