#ifndef BATTALION_TABLE_TYPES_H
#define BATTALION_TABLE_TYPES_H

#include "field_entry_internal.h"

typedef struct BattalionTable BattalionTable;

// static table
typedef struct BattalionStaticTable BattalionStaticTable;
typedef struct BattalionStaticTableDiff BattalionStaticTableDiff;

// dynamic tables
typedef struct BattalionDynamicTable BattalionDynamicTable;
typedef struct BattalionDynamicTableDiff BattalionDynamicTableDiff;

enum HeadlessMovementMode {

    UNKNOWN_HEADLESS_MOVEMENT_MODE = -1,
    HEADLESS_HOLD,
    HEADLESS_DRIFT,
    HEADLESS_CHARGE

};

extern const ComplexFieldEntry *const battalion_spreadsheet_field_table_ptr;

extern const ComplexFieldEntry *const battalion_static_table_field_table_ptr;
extern const ComplexFieldEntry *const battalion_static_table_diff_field_table_ptr;
extern const ComplexFieldEntry *const battalion_dynamic_table_field_table_ptr;
extern const ComplexFieldEntry *const battalion_dynamic_table_diff_field_table_ptr;
extern const ComplexFieldEntry *const battalion_comparison_table_field_table_ptr;

#endif