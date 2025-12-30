#include "player.h"
#include <stdio.h>

static Player players[MAX_PLAYERS];
static int player_count;

int create_player(int max_hp, int max_st, const char *name, float spawnX, float spawnY, float dimensionX, float dimensionY, bool collision, int quadrant) {
    if (player_count >= MAX_PLAYERS) return -1; 
    int id = player_count;
    players[id].id = id;
    players[id].max_hp = max_hp;
    players[id].max_st = max_st;
    players[id].current_hp = max_hp;
    players[id].current_st = max_st;
    snprintf(players[id].name, sizeof players[id].name, "%s", name ? name : "");
    players[id].spawnX = spawnX;
    players[id].spawnY = spawnY;
    players[id].dimensionX = dimensionX;
    players[id].dimensionY = dimensionY;
    players[id].collision = collision;
    players[id].quadrant = quadrant;
    player_count++;
    return id;
}

Player *get_player(int id){
    if (id < 0 || id >= player_count) return NULL;
    return &players[id];
}