#ifndef PLAYER
#define PLAYER

#include <stdbool.h>

#define MAX_PLAYERS 16

typedef struct Player {
    int id;
    int max_hp;
    int max_st;
    int current_hp;
    int current_st;
    char name[32];
    float playerSpawnX, playerSpawnY;
    float playerPositionX, playerPositionY;
    float playerDimensionX, playerDimensionY;
    bool collision;
    int quadrant;
    float playerWindowPositionX;
    float playerWindowPositionY;

} Player;

int create_player(int max_hp, int max_st, const char *name,
                   float playerSpawnX, float playerSpawnY,
                   float playerPositionX, float playerDimensionY,
                   float playerDimensionX, float playerDimensionY,
                   bool collision, int quadrant,
                   float playerWindowPositionX, float playerWindowPositionY);

Player *get_player(int id);

#endif