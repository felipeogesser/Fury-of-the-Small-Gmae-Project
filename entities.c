#include "entities.h"

Entity *entities = NULL;
void init_entities(void)
{
    entities = malloc(MAX_ENTITIES * sizeof *entities);
}
static int entity_count = 0;

int create_entity(const char *name, const char *team, float positionX, float positionY,
    float dimensionX, float dimensionY,
    int R_Color, int G_Color, int B_Color, int Alpha,
    bool collision, int speed) {
        if (entity_count >= MAX_ENTITIES) return -1;
        int index = entity_count;
        entities[index].id = index + 1;
        snprintf(entities[index].name, sizeof entities[index].name, "%s", name);
        snprintf(entities[index].team, sizeof entities[index].team, "%s", team);
        entities[index].positionX = positionX;
        entities[index].positionY = positionY;
        entities[index].dimensionX = dimensionX;
        entities[index].dimensionY = dimensionY;
        entities[index].R_Color = R_Color;
        entities[index].G_Color = G_Color;
        entities[index].B_Color = B_Color;
        entities[index].Alpha = Alpha;
        entities[index].collision = collision;
        entities[index].speed = speed;
        entity_count++;
        return entities[index].id;
    }

Entity *get_entity(int id) {
    if (id <= 0 || id > entity_count) return NULL;
    return &entities[id - 1];
}