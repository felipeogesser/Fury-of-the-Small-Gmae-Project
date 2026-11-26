#include "starting_army.h"

static const StartingArmyFile files[STARTING_ARMY_COUNT] = {
    [STARTING_ARMY_TRAINING] = {"inventory.json", 9}
};

StartingArmyFile starting_army_file(enum StartingArmy army) {

    if (army < 0 || army >= STARTING_ARMY_COUNT) army = STARTING_ARMY_TRAINING;
    return files[army];

}
