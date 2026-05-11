#ifndef BAKING_H
#define BAKING_H

#include "calculateEntityQuadrant.h"
#include "grids.h"
#include "loadArmies.h"
#include "mapMaker.h"
#include "maps.h"
#include "quadrant.h"

void bake_map(void);

void bake_player(void);

void bake_game_state(void);

void bake_armies(void);

void bake_grids(void);

void bake_entity_quad(void);

void data_baking(void);

#endif