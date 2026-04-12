#include "entities.h"

Entity *entities = NULL;
void init_entities(void)
{
    entities = malloc(MAX_ENTITIES * sizeof *entities);
}
static int entity_count = 1;

int create_entity(const char *name, float positionX, float positionY,
    float dimensionX, float dimensionY,
    int R_Color, int G_Color, int B_Color, int Alpha,
    bool collision, int speed) {
        if (entity_count >= MAX_ENTITIES) return -1;
        int id = entity_count;
        entities[id].id = id;
        snprintf(entities[id].name, sizeof entities[id].name, "%s", name);
        entities[id].positionX = positionX;
        entities[id].positionY = positionY;
        entities[id].dimensionX = dimensionX;
        entities[id].dimensionY = dimensionY;
        entities[id].R_Color = R_Color;
        entities[id].G_Color = G_Color;
        entities[id].B_Color = B_Color;
        entities[id].Alpha = Alpha;
        entities[id].collision = collision;
        entities[id].speed = speed;
        entity_count++;
        return id;
    }



Entity *get_entity(int id) {
    if (id < 0 || id >= entity_count) return NULL;
    return &entities[id];
}