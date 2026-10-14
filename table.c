#include "table.h"
#include "table_internal.h"
#include "battalion_table.h"
#include "general_table.h"
#include "unit_table.h"

void clear_table_vectors(Table *tables, unsigned int battalion_count, unsigned int battalion_size) {

    battalion_count /= 2;
    unsigned int unit_count = battalion_count * battalion_size;
    for (unsigned int idx = 0, i = 0; i < unit_count; i++) {

        idx = i / battalion_size;
        tables->general_tables.friendly_dynamic.vec_x[idx] = 0.0f;
        tables->general_tables.friendly_dynamic.vec_y[idx] = 0.0f;
        tables->unit_tables.friendly_dynamic.vec_x[i] = 0.0f;
        tables->unit_tables.friendly_dynamic.vec_y[i] = 0.0f;

    }

}

void table_destroy(void) {

    unit_table_destroy();
    general_table_destroy();
    battalion_table_destroy();

}