#ifndef LOAD_ARMIES_H
#define LOAD_ARMIES_H

#include "armies_types.h"
#include "battleplan_battlefield_handoff.h"
#include <stddef.h>

void init_armies_memory_arena(GeneralPayload **general_payload, unsigned int armies_count, unsigned int battalion_size);
Armies *load_armies_into_arena(GeneralPayload **general_payload, unsigned int armies_count, unsigned int battalion_size);
void free_army_memory(void);

extern size_t armies_total_memory_size;

#endif