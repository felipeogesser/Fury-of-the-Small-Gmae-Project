#include "ai_order.h"
#include "ai_order_internal.h"
#include "field_entry.h"
#include "field_entry_internal.h"
#include "memory_arena.h"
#include "table_types.h"
#include <stddef.h>

// private prototypes
static void allocate_soa_memory(
    unsigned int obj_count,
    BattalionOrderTable *player_order_table,
    BattalionOrderTable *enemy_order_table,
    ComplexFieldEntry *battalion_order_table_field_table_ptr);
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

void battalion_order_table_init(Table *player_tables, Table *enemy_tables, unsigned int battalion_count) {

    BattalionOrderTable player_order_table = {0};
    BattalionOrderTable enemy_order_table = {0};

    allocate_soa_memory(
        battalion_count,
        &player_order_table,
        &enemy_order_table,
        battalion_order_table_field_table_ptr
    );

    player_tables->battalion_order = (BattalionOrderTable)player_order_table;
    enemy_tables->battalion_order = (BattalionOrderTable)enemy_order_table;

}

static void allocate_soa_memory(
    unsigned int obj_count,
    BattalionOrderTable *player_order_table,
    BattalionOrderTable *enemy_order_table,
    ComplexFieldEntry *battalion_order_table_field_table_ptr) {

    battalion_order_table_start_address_alloc = memory_arena_current_pointer();
    allocate_mem(obj_count, battalion_order_table_field_table_ptr, player_order_table);
    allocate_mem(obj_count, battalion_order_table_field_table_ptr, enemy_order_table);

}

static void allocate_mem(unsigned int obj_count, const ComplexFieldEntry *field_table, void *order_table) {

    unsigned int fields_count = complex_field_table_fields_count(field_table);
    size_t size = 0;
    size_t aligment = 64;
    for (unsigned int i = 0; i < fields_count; i++) {

        size = field_table[i].size * obj_count;
        //aligment = size & -size;
        //aligment = (alignment <= 64) ? aligment : 64;

        size_t size_padded = (size + 63) & ~(size_t)63;
        void **dest = &((char *)mock + field_table[i].offset);
        *dest = memory_arena_push(size_padded, alignment);

    }

}