#include "stage_enemy.h"
#include "deployment_loader.h"
#include "run.h"
#include "stage_army.h"
#include "tutorial.h"

#define STAGE_ENEMY_TUTORIAL_LAYOUT "tutorial.csv"

void stage_enemy_load(Battleplan *battleplan) {

    if (tutorial_is_active() || !run_is_active()) {
        load_generals_from_csv_deployment_layout_to_enemy_board(battleplan, STAGE_ENEMY_TUTORIAL_LAYOUT);
        return;
    }

    unsigned int layout[GRID_DIMENSION_X][GRID_DIMENSION_Y];
    stage_army_layout(run_current_stage(), run_seed(), layout);
    load_generals_from_layout_to_enemy_board(battleplan, layout);

}
