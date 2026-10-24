#include "ai_order.h"
#include "ai_order_internal.h"
#include "field_entry.h"
#include "field_entry_internal.h"
#include "memory_arena.h"
#include "table_internal.h"
#include <stddef.h>

// private prototypes
static void allocate_soa_memory(
    unsigned int obj_count,
    BattalionOrderTable *player_order_table,
    BattalionOrderTable *enemy_order_table);
static void allocate_mem(unsigned int obj_count, const ComplexFieldEntry *field_table, void *order_table);

static void *battalion_order_table_start_address_alloc = NULL;

#define OFFSET_OF(type, member) offsetof(type, member)
#define SIZE_OF(type, member) (sizeof(((type *)0)->member))
#define ELEMENT_SIZE_OF(type, member) (sizeof(*(((type *)0)->member)))

static const ComplexFieldEntry battalion_order_table_field_table[] = {

    { "type",    OFFSET_OF(BattalionOrderTable, type),    ELEMENT_SIZE_OF(BattalionOrderTable, type),    S_INT, NULL },
    { "pace",    OFFSET_OF(BattalionOrderTable, pace),    ELEMENT_SIZE_OF(BattalionOrderTable, pace),    S_INT, NULL },
    { "target",  OFFSET_OF(BattalionOrderTable, target),  ELEMENT_SIZE_OF(BattalionOrderTable, target),  U_INT, NULL },
    { "dest_x",  OFFSET_OF(BattalionOrderTable, dest_x),  ELEMENT_SIZE_OF(BattalionOrderTable, dest_x),  FLOAT, NULL },
    { "dest_y",  OFFSET_OF(BattalionOrderTable, dest_y),  ELEMENT_SIZE_OF(BattalionOrderTable, dest_y),  FLOAT, NULL },
    { "obj_size", 0, sizeof(BattalionOrderTable), NOT_DEFINED, NULL }

};
const ComplexFieldEntry *const battalion_order_table_field_table_ptr = battalion_order_table_field_table;

#undef OFFSET_OF
#undef SIZE_OF
#undef ELEMENT_SIZE_OF

static BattalionOrderTable *player_order_table_ptr = NULL;
static BattalionOrderTable *enemy_order_table_ptr = NULL;
void battalion_order_table_init(Table *player_tables, Table *enemy_tables, unsigned int battalion_count) {

    BattalionOrderTable player_order_table = {0};
    BattalionOrderTable enemy_order_table = {0};

    allocate_soa_memory(battalion_count, &player_order_table, &enemy_order_table);

    player_tables->battalion_order = player_order_table;
    enemy_tables->battalion_order = enemy_order_table;

    player_order_table_ptr = &player_tables->battalion_order;
    enemy_order_table_ptr = &enemy_tables->battalion_order;

}

void battalion_order_state_player_order(unsigned int battalion_idx, enum OrderType type, enum BattalionMovementPace pace, unsigned int target, float dest_x, float dest_y) {

    player_order_table_ptr->type[battalion_idx] = type;
    player_order_table_ptr->pace[battalion_idx] = pace;
    player_order_table_ptr->target[battalion_idx] = target;
    player_order_table_ptr->dest_x[battalion_idx] = dest_x;
    player_order_table_ptr->dest_y[battalion_idx] = dest_y;

}

void battalion_order_state_enemy_order(unsigned int battalion_idx, enum OrderType type, enum BattalionMovementPace pace, unsigned int target, float dest_x, float dest_y) {

    enemy_order_table_ptr->type[battalion_idx] = type;
    enemy_order_table_ptr->pace[battalion_idx] = pace;
    enemy_order_table_ptr->target[battalion_idx] = target;
    enemy_order_table_ptr->dest_x[battalion_idx] = dest_x;
    enemy_order_table_ptr->dest_y[battalion_idx] = dest_y;

}

static void allocate_soa_memory(
    unsigned int obj_count,
    BattalionOrderTable *player_order_table,
    BattalionOrderTable *enemy_order_table) {

    battalion_order_table_start_address_alloc = memory_arena_current_pointer();
    allocate_mem(obj_count, battalion_order_table_field_table_ptr, player_order_table);
    allocate_mem(obj_count, battalion_order_table_field_table_ptr, enemy_order_table);

}

static void allocate_mem(unsigned int obj_count, const ComplexFieldEntry *field_table, void *order_table) {

    unsigned int fields_count = complex_field_table_fields_count(field_table);
    size_t size = 0;
    size_t alignment = 64;
    for (unsigned int i = 0; i < fields_count; i++) {

        size = field_table[i].size * obj_count;
        size_t size_padded = (size + 63) & ~(size_t)63;
        void **dest = (void **)((char *)order_table + field_table[i].offset);
        *dest = memory_arena_push(size_padded, alignment);

    }

}