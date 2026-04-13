#ifndef ENTITYMAKER
#define ENTITYMAKER

#include "entities.h"
#include <stdio.h>

extern int entity_id[MAX_ENTITIES];

void make_entities(void);
void auto_make_entities(int iter);
void team_blue_entities(int iter);
void team_red_entities(int iter);

#endif