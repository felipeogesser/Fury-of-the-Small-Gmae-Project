#ifndef PLAYER
#define PLAYER

#include "game_state_types.h"
#include "player_types.h"

//void init_player(void);

unsigned int create_player(int max_hp, int max_st, const char *name,
                   float playerSpawnX, float playerSpawnY,
                   float playerDimensionX, float playerDimensionY,
                   _Bool collision, int quadrant,
                   float speed, float runSpeed);

Player *get_player(unsigned int playerId);

void calculate_player_movement(GameState *game);

void update_player(GameState *game);

#endif