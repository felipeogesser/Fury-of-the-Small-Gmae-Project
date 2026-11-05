#ifndef GENERAL_TABLE_H
#define GENERAL_TABLE_H

#include "battlefield_types.h"
#include "general_types.h"
#include "maps_types.h"
#include "table_types.h"

void general_table_init(Table *player_tables, Table *enemy_tables, General *player_general, General *enemy_general, unsigned int battalion_count);
void general_table_update(Table *tables, Map *map, enum ArmyBattlefieldSide side, unsigned int battalion_count, unsigned int battalion_size, float dt);
void general_table_render(Table *tables, unsigned int i);
void general_table_destroy(void);

#endif