#include "ai_order_span.h"
#include "ai_order_internal.h"
#include "shared_memory_arena.h"

#define SPAN(base, table, field, count) \
    ((Span){ \
        (uint64_t)((const char *)(table)->field - (const char *)(base)), \
        (uint64_t)((size_t)(count) * sizeof *(table)->field) \
    })

static void battalion_order_span_set(BattalionOrderTableSpan *s, const BattalionOrderTable *t, const void *base, size_t n) {

    s->type = SPAN(base, t, type, n);
    s->pace = SPAN(base, t, pace, n);
    s->target = SPAN(base, t, target, n);
    s->dest_x = SPAN(base, t, dest_x, n);
    s->dest_y = SPAN(base, t, dest_y, n);

}

#undef SPAN

void set_order_table_shared_memory_span(BattalionOrderTableSpan *player_spans, BattalionOrderTableSpan *enemy_spans, const Table *player_tables, const Table *enemy_tables, size_t battalion_count) {

    const void *arena_base = shared_memory_arena_base();

    battalion_order_span_set(player_spans, &player_tables->battalion_order, arena_base, battalion_count);
    battalion_order_span_set(enemy_spans, &enemy_tables->battalion_order, arena_base, battalion_count);

}