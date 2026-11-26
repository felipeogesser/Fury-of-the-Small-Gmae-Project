#ifndef STARTING_ARMY_H
#define STARTING_ARMY_H

enum StartingArmy {
    STARTING_ARMY_TRAINING,
    STARTING_ARMY_COUNT
};

typedef struct StartingArmyFile {

    const char *path;
    unsigned int general_count;

} StartingArmyFile;

StartingArmyFile starting_army_file(enum StartingArmy army);

#endif
