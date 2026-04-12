#ifndef ENTITYMAKER
#define ENTITYMAKER

#include "entities.h"
#include <stdio.h>

extern int entity_id[MAX_ENTITIES];
extern int I;

void make_entities(void);
void auto_make_entities(int iter);

#endif