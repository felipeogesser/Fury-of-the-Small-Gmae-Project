#include "player.h"

static Player players[MAX_PLAYERS];
static int player_count;

int create_player(int max_hp, int max_st, const char *name,
    float playerSpawnX, float playerSpawnY,
    float playerDimensionX, float playerDimensionY,
    bool collision, int quadrant,
    float speed, float runSpeed) {
    if (player_count >= MAX_PLAYERS) return -1; 
    int id = player_count;
    players[id].id = id;
    players[id].max_hp = max_hp;
    players[id].max_st = max_st;
    players[id].current_hp = max_hp;
    players[id].current_st = max_st;
    snprintf(players[id].name, sizeof players[id].name, "%s", name ? name : "");
    players[id].playerSpawnX = playerSpawnX;
    players[id].playerSpawnY = playerSpawnY;
    players[id].playerPositionX = playerSpawnX;
    players[id].playerPositionY = playerSpawnY;
    players[id].playerDimensionX = playerDimensionX;
    players[id].playerDimensionY = playerDimensionY;
    players[id].collision = collision;
    players[id].quadrant = quadrant;
    players[id].playerWindowPositionX = WINDOW_SIZE_X / 2 - playerDimensionX / 2;
    players[id].playerWindowPositionY = WINDOW_SIZE_Y / 2 - playerDimensionY / 2;
    players[id].speed = speed;
    players[id].runSpeed = runSpeed;
    player_count++;
    return id;
}

Player *get_player(int id){
    if (id < 0 || id >= player_count) return NULL;
    return &players[id];
}