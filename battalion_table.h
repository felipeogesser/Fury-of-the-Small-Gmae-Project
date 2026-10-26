#ifndef BATTALION_TABLE_H
#define BATTALION_TABLE_H

#include "battalion_types.h"
#include "field_entry_types.h"
#include "spreadsheet_types.h"
#include "table_types.h"

void battalion_table_init(Table *player_tables, Table *enemy_tables, Battalion *player_battalion, Battalion *enemy_battalion,/* const ComplexFieldEntry *battalion_field_table,*/ unsigned int battalion_count);
void battalion_table_update(const Spreadsheet *spreadsheet, Table *tables, signed int balance_of_power, unsigned int battalion_count, unsigned int battalion_size, float dt);
void battalion_table_destroy(void);

#endif