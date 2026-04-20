#include "player.h"

static Player players[MAX_PLAYERS];
static int player_count = 0;

int create_player(int max_hp, int max_st, const char *name,
    float playerSpawnX, float playerSpawnY,
    float playerDimensionX, float playerDimensionY,
    bool collision, int quadrant,
    float speed, float runSpeed) {
    if (player_count >= MAX_PLAYERS) return -1; 
    int playerIndex = player_count;
    players[playerIndex].id = playerIndex + 1;
    players[playerIndex].max_hp = max_hp;
    players[playerIndex].max_st = max_st;
    players[playerIndex].current_hp = max_hp;
    players[playerIndex].current_st = max_st;
    snprintf(players[playerIndex].name, sizeof players[playerIndex].name, "%s", name ? name : "");
    players[playerIndex].playerSpawnX = playerSpawnX;
    players[playerIndex].playerSpawnY = playerSpawnY;
    players[playerIndex].playerPositionX = playerSpawnX;
    players[playerIndex].playerPositionY = playerSpawnY;
    players[playerIndex].playerDimensionX = playerDimensionX;
    players[playerIndex].playerDimensionY = playerDimensionY;
    players[playerIndex].collision = collision;
    players[playerIndex].quadrant = quadrant;
    players[playerIndex].playerWindowPositionX = WINDOW_SIZE_X / 2 - playerDimensionX / 2;
    players[playerIndex].playerWindowPositionY = WINDOW_SIZE_Y / 2 - playerDimensionY / 2;
    players[playerIndex].speed = speed;
    players[playerIndex].runSpeed = runSpeed;
    player_count++;
    return players[playerIndex].id;
}

Player *get_player(int playerId){
    if (playerId <= 0 || playerId > player_count) return NULL;
    return &players[playerId - 1];
}