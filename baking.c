#include "baking.h"
#include "battalion_table_span.h"
#include "engine_internal.h"
#include "shared_memory_control.h"
#include "sprites.h"

static void *baked_data_end_ptr = NULL;

void data_baking(void) {

    shared_memory_control_base_ptr = shared_memory_control_create();
    battalion_table_span_base_ptr = shared_memory_span_create(shared_memory_control_base_ptr);
    order_buffer_base_ptr = shared_memory_order_create(shared_memory_control_base_ptr);

    load_sprites_into_memory();

    baked_data_end_ptr = memory_arena_current_pointer();

}

void *data_baking_end_pointer(void) {

    return baked_data_end_ptr;

}