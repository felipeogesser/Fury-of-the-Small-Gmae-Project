#ifndef BATTLEPLAN_INTERNAL_H
#define BATTLEPLAN_INTERNAL_H

#include "battleplan_types.h"
#include "battleplan_grid_types.h"
#include "general_types.h"

typedef struct Background {

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

} Background;

typedef struct Button {

    const char *text;
    unsigned short x;
    unsigned short y;
    unsigned short w;
    unsigned short h;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

} Button;

typedef struct DrawerSlot {

    General *general;
    float x;
    float y;
    float w;
    float h;

} DrawerSlot;

/*typedef struct EnemyBoard {

    General *general;
    General *grid[GRID_DIMENSION_X][GRID_DIMENSION_Y];
    unsigned int general_count;
    _Bool occupied_grid_slot[GRID_DIMENSION_X * GRID_DIMENSION_Y];

} EnemyBoard;*/

typedef struct Board {

    General *grid[GRID_DIMENSION_X][GRID_DIMENSION_Y];
    unsigned int general_count;
    
} Board;

/*typedef struct Battleplan {

    Background background;
    Button button_main_menu;
    Button button_init_battle;
    unsigned int general_in_grid_count;
    EnemyBoard enemy_board;

} Battleplan;*/

typedef struct Battleplan {

    Background background;
    Button button_main_menu;
    Button button_init_battle;
    //unsigned int general_in_grid_count;
    Board player_board;
    Board enemy_board;
    General *player_inventory_general; // needs manual free();
    General *stage_enemy_general; // needs manual free();

} Battleplan;

#endif