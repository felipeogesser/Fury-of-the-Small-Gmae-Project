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
    float spawnX, spawnY;
    float dimensionX, dimensionY;
    bool collision;
    int quadrant;
    float PlayerWindowPositionX;
    float PlayerWindowPositionY;

} Player;

int create_player(int max_hp, int max_st, const char *name,
                   float spawnX, float spawnY, float dimensionX, float dimensionY,
                   bool collision, int quadrant);

Player *get_player(int id);

#endif