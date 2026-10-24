#ifndef BATTALION_TABLE_SPAN_H
#define BATTALION_TABLE_SPAN_H

#include <stdint.h>
#include <stddef.h>

#include "table_internal.h"

typedef struct Span {
    uint64_t offset;
    uint64_t size;
} Span;

typedef struct BattalionStaticTableSpan {
    Span battalion_type;
    Span range;
    Span charge_bonus;
    Span armor;
    Span max_hp;
    Span attack_speed;
    Span start_entity_count;
    Span formation_x;
    Span formation_y;
    Span is_shielded;
    Span is_ranged;
    Span is_large;
    Span is_armor_piercing;
    Span is_anti_large;
} BattalionStaticTableSpan;

typedef struct BattalionDynamicTableSpan {
    Span headless_movement_mode;
    Span fatigue_state;
    Span morale_state;
    Span lifecycle_state;
    Span movement_state;
    Span movement_pace;
    Span target_battalion_idx;
    Span morale_modifiers;
    Span withdrawed_entity_count;
    Span dead_entity_count;
    Span alive_entity_count;
    Span unit_alive_count;
    Span target_x;
    Span target_y;
    Span x;
    Span y;
    Span morale;
    Span fatigue;
    Span hp;
    Span speed;
    Span meele_attack;
    Span meele_defense;
    Span strength;
    Span rally_timer;
    Span charge_timer;
    Span is_active;
    Span is_rallying;
    Span is_charging;
    Span is_engaging;
    Span is_engaged;
    Span is_controllable;
    Span is_present;
    Span is_headless;
    Span is_bracing;
} BattalionDynamicTableSpan;

typedef struct BattalionTableSpan {
    BattalionStaticTableSpan friendly_static;
    BattalionStaticTableSpan enemy_static;
    BattalionDynamicTableSpan friendly_dynamic;
    BattalionDynamicTableSpan enemy_dynamic;
} BattalionTableSpan;

_Static_assert(sizeof(Span) == 16, "Span size changed");
_Static_assert(sizeof(BattalionStaticTableSpan) == 14 * 16, "BattalionStaticTableSpan size changed");
_Static_assert(sizeof(BattalionDynamicTableSpan) == 34 * 16, "BattalionDynamicTableSpan size changed");
_Static_assert(sizeof(BattalionTableSpan) == (2 * 14 + 2 * 34) * 16, "BattalionTableSpan size changed");

void set_table_shared_memory_span(BattalionTableSpan *spans, const Table *tables, size_t battalion_count);

extern void *battalion_table_span_base_ptr;

#endif