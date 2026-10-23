#ifndef DEPLOYMENT_LOADER_H
#define DEPLOYMENT_LOADER_H

#include "battleplan_types.h"

void load_generals_from_csv_deployment_layout_to_player_board(Battleplan *battleplan, const char *file_name);
void load_generals_from_csv_deployment_layout_to_enemy_board(Battleplan *battleplan, const char *file_name);

#endif