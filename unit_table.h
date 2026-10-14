#ifndef UNIT_TABLE_H
#define UNIT_TABLE_H

#include "unit_types.h"
#include "battlefield_types.h"
#include "table_types.h"

void unit_table_init(Table *player_tables, Table *enemy_tables, Unit *player_unit, Unit *enemy_unit, unsigned int battalion_count, unsigned int battalion_size);
void unit_table_update(Table *tables, unsigned int battalion_count, unsigned int battalion_size, enum ArmyBattlefieldSide side, float dt);
void unit_table_render(Table *tables, unsigned int battalion_count, unsigned int battalion_size);
void unit_table_destroy(void);

#endif