#ifndef AI_ORDER_H
#define AI_ORDER_H

#include "ai_order_types.h"
#include "battalion_types.h"
#include "table_types.h"

void battalion_order_table_init(Table *player_tables, Table *enemy_tables, unsigned int battalion_count);
void battalion_order_state_player_order(unsigned int battalion_idx, enum OrderType type, enum BattalionMovementPace pace, unsigned int target, float dest_x, float dest_y);
void battalion_order_state_enemy_order(unsigned int battalion_idx, enum OrderType type, enum BattalionMovementPace pace, unsigned int target, float dest_x, float dest_y);

#endif