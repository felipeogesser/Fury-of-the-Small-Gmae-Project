#ifndef PLAYER_INTERNAL_H
#define PLAYER_INTERNAL_H

#define MAX_PLAYERS 1

typedef struct Player {
    unsigned int id;
    int max_hp;
    int max_st;
    int current_hp;
    int current_st;
    char name[32];
    float vx, vy, vxdt, vydt, va, vb;
    float dir_left, dir_right, dir_up, dir_down;
    float playerSpawnX, playerSpawnY;
    float playerPositionX, playerPositionY;
    float playerPositionOnScreenX, playerPositionOnScreenY;
    float playerDimensionX, playerDimensionY;
    _Bool collision;
    int quadrant;
    float speed;
    float runSpeed;
} Player;

#endif