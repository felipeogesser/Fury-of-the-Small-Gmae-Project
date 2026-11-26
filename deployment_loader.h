#ifndef DEPLOYMENT_LOADER_H
#define DEPLOYMENT_LOADER_H

#include "battleplan_types.h"
#include "battleplan_grid_types.h"
#include "general_types.h"

void load_generals_from_csv_deployment_layout_to_player_board(Battleplan *battleplan, const char *file_name);
void load_generals_from_csv_deployment_layout_to_enemy_board(Battleplan *battleplan, const char *file_name);
void load_generals_from_layout_to_enemy_board(Battleplan *battleplan, unsigned int (*layout)[GRID_DIMENSION_Y]);
General convert_spreadsheet_general_to_general(enum GeneralType general_type);

#endif