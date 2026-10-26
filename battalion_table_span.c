#include "battalion_table_span.h"
#include "battalion_table_internal.h"
#include "memory_arena.h"

_Static_assert(sizeof(_Bool) == 1, "_Bool must be 1 byte");
_Static_assert(sizeof(unsigned char) == 1, "unsigned char must be 1 byte");
_Static_assert(sizeof(unsigned int) == 4, "unsigned int must be 4 bytes");
_Static_assert(sizeof(float) == 4, "float must be 4 bytes");
_Static_assert(sizeof(enum BattalionType) == 4, "enum BattalionType must be 4 bytes");
_Static_assert(sizeof(enum HeadlessMovementMode) == 4, "enum HeadlessMovementMode must be 4 bytes");
_Static_assert(sizeof(enum FatigueState) == 4, "enum FatigueState must be 4 bytes");
_Static_assert(sizeof(enum MoraleState) == 4, "enum MoraleState must be 4 bytes");
_Static_assert(sizeof(enum BattalionLifecycle) == 4, "enum BattalionLifecycle must be 4 bytes");
_Static_assert(sizeof(enum BattalionMovementMode) == 4, "enum BattalionMovementMode must be 4 bytes");
_Static_assert(sizeof(enum BattalionMovementPace) == 4, "enum BattalionMovementPace must be 4 bytes");

#define SPAN(base, table, field, count) \
    ((Span){ \
        (uint64_t)((const char *)(table)->field - (const char *)(base)), \
        (uint64_t)((size_t)(count) * sizeof *(table)->field) \
    })

static void battalion_static_span_set(BattalionStaticTableSpan *s, const BattalionStaticTable *t, const void *base, size_t n) {

    s->battalion_type = SPAN(base, t, battalion_type, n);
    s->range = SPAN(base, t, range, n);
    s->charge_bonus = SPAN(base, t, charge_bonus, n);
    s->armor = SPAN(base, t, armor, n);
    s->max_hp = SPAN(base, t, max_hp, n);
    s->attack_speed = SPAN(base, t, attack_speed, n);
    s->start_entity_count = SPAN(base, t, start_entity_count, n);
    s->formation_x = SPAN(base, t, formation_x, n);
    s->formation_y = SPAN(base, t, formation_y, n);
    s->is_shielded = SPAN(base, t, is_shielded, n);
    s->is_ranged = SPAN(base, t, is_ranged, n);
    s->is_large = SPAN(base, t, is_large, n);
    s->is_armor_piercing = SPAN(base, t, is_armor_piercing, n);
    s->is_anti_large = SPAN(base, t, is_anti_large, n);

}

static void battalion_dynamic_span_set(BattalionDynamicTableSpan *s, const BattalionDynamicTable *t, const void *base, size_t n) {

    s->headless_movement_mode = SPAN(base, t, headless_movement_mode, n);
    s->fatigue_state = SPAN(base, t, fatigue_state, n);
    s->morale_state = SPAN(base, t, morale_state, n);
    s->lifecycle_state = SPAN(base, t, lifecycle_state, n);
    s->movement_state = SPAN(base, t, movement_state, n);
    s->movement_pace = SPAN(base, t, movement_pace, n);
    s->target_battalion_idx = SPAN(base, t, target_battalion_idx, n);
    s->morale_modifiers = SPAN(base, t, morale_modifiers, n);
    s->withdrawed_entity_count = SPAN(base, t, withdrawed_entity_count, n);
    s->dead_entity_count = SPAN(base, t, dead_entity_count, n);
    s->alive_entity_count = SPAN(base, t, alive_entity_count, n);
    s->unit_alive_count = SPAN(base, t, unit_alive_count, n);
    s->target_x = SPAN(base, t, target_x, n);
    s->target_y = SPAN(base, t, target_y, n);
    s->x = SPAN(base, t, x, n);
    s->y = SPAN(base, t, y, n);
    s->morale = SPAN(base, t, morale, n);
    s->fatigue = SPAN(base, t, fatigue, n);
    s->hp = SPAN(base, t, hp, n);
    s->speed = SPAN(base, t, speed, n);
    s->meele_attack = SPAN(base, t, meele_attack, n);
    s->meele_defense = SPAN(base, t, meele_defense, n);
    s->strength = SPAN(base, t, strength, n);
    s->rally_timer = SPAN(base, t, rally_timer, n);
    s->charge_timer = SPAN(base, t, charge_timer, n);
    s->is_active = SPAN(base, t, is_active, n);
    s->is_rallying = SPAN(base, t, is_rallying, n);
    s->is_charging = SPAN(base, t, is_charging, n);
    s->is_engaging = SPAN(base, t, is_engaging, n);
    s->is_engaged = SPAN(base, t, is_engaged, n);
    s->is_controllable = SPAN(base, t, is_controllable, n);
    s->is_present = SPAN(base, t, is_present, n);
    s->is_headless = SPAN(base, t, is_headless, n);
    s->is_bracing = SPAN(base, t, is_bracing, n);

}

#undef SPAN

void set_table_shared_memory_span(BattalionTableSpan *spans, const Table *tables, size_t battalion_count) {

    const void *arena_base = memory_arena_base();
    const BattalionTable *bt = &tables->battalion_tables;

    battalion_static_span_set(&spans->friendly_static, &bt->friendly_static, arena_base, battalion_count);
    battalion_static_span_set(&spans->enemy_static, &bt->enemy_static, arena_base, battalion_count);

    battalion_dynamic_span_set(&spans->friendly_dynamic, &bt->friendly_dynamic, arena_base, battalion_count);
    battalion_dynamic_span_set(&spans->enemy_dynamic, &bt->enemy_dynamic, arena_base, battalion_count);

}