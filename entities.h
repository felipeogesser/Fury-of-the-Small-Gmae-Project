#ifndef ENTITIES
#define ENTITIES

#include "armies_types.h"
#include "entities_types.h"
#include "game_state_types.h"
#include "general_types.h"

int create_entity(const char *name, const char *team, float positionX, float positionY,
    float dimensionX, float dimensionY, _Bool collision, int speed);
    
Entity *get_entity(int id);

void init_entities(void);

void create_entities(Entity *entities, General *general, float ongoing_point_X, float ongoing_point_Y, unsigned int i, unsigned int k);

void update_units(Armies *armies, GameState *game);

#endif