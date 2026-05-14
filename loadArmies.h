#ifndef LOAD_ARMIES_H
#define LOAD_ARMIES_H

#include <stddef.h>

void init_armies_memory_arena(void);
void load_armies_into_arena(void);
void free_army_memory(void);

extern size_t armies_total_memory_size;

#endif