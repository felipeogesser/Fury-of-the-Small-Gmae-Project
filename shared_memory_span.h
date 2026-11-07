#ifndef SHARED_MEMORY_SPAN_H
#define SHARED_MEMORY_SPAN_H

#include "battalion_table_span.h"

typedef struct SharedMemorySpan {
    BattalionTableSpan battalion;
    BattalionOrderTableSpan player_order;
    BattalionOrderTableSpan enemy_order;
} SharedMemorySpan;

_Static_assert(sizeof(SharedMemorySpan) == (2 * 14 + 2 * 34 + 2 * 5) * 16, "SharedMemorySpan size changed");

#endif