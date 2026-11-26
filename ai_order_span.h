#ifndef AI_ORDER_SPAN_H
#define AI_ORDER_SPAN_H

#include <stddef.h>

#include "battalion_table_span.h"
#include "table_internal.h"

typedef struct BattalionOrderTableSpan {
    Span type;
    Span pace;
    Span target;
    Span dest_x;
    Span dest_y;
} BattalionOrderTableSpan;

_Static_assert(sizeof(BattalionOrderTableSpan) == 5 * 16, "BattalionOrderTableSpan size changed");

void set_order_table_shared_memory_span(BattalionOrderTableSpan *player_spans, BattalionOrderTableSpan *enemy_spans, const Table *player_tables, const Table *enemy_tables, size_t battalion_count);

#endif