#ifndef PLAYER
#define PLAYER

#define MAX_PLAYERS 1

#include <stdio.h>
#include <stdbool.h>
#include "windowSettings.h"
#include "gameState.h"

typedef struct Player {
    int id;
    int max_hp;
    int max_st;
    int current_hp;
    int current_st;
    char name[32];
    float playerSpawnX, playerSpawnY;
    float playerPositionX, playerPositionY;
    float playerPositionOnScreenX, playerPositionOnScreenY;
    float playerDimensionX, playerDimensionY;
    bool collision;
    int quadrant;
    float speed;
    float runSpeed;
} Player;

//void init_player(void);

int create_player(int max_hp, int max_st, const char *name,
                   float playerSpawnX, float playerSpawnY,
                   float playerDimensionX, float playerDimensionY,
                   bool collision, int quadrant,
                   float speed, float runSpeed);

Player *get_player(int playerId);

void calculate_player_movement(GameState *game);

#endif