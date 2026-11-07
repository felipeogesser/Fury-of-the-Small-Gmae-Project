#ifndef TABLE_INTERNAL_H
#define TABLE_INTERNAL_H

#include "ai_order_internal.h"
#include "battalion_table_internal.h"
#include "general_table_internal.h"
#include "unit_table_internal.h"

typedef struct Table {

    BattalionTable battalion_tables;

    GeneralTable general_tables;
    
    UnitTable unit_tables;

    BattalionOrderTable battalion_order;

} Table;

#endif