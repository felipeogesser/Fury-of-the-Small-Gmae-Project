#include "entities.h"

Entity *entities = NULL;
void init_entities(void)
{
    entities = calloc(MAX_ENTITIES, sizeof *entities);
}
static int entity_count = 0;

int create_entity(const char *name, const char *team, float positionX, float positionY,
    float dimensionX, float dimensionY,
    int R_Color, int G_Color, int B_Color, int Alpha,
    bool collision, int speed) {
        if (entity_count >= MAX_ENTITIES) return -1;
        int entityIndex = entity_count;
        entities[entityIndex].id = entityIndex + 1;
        snprintf(entities[entityIndex].name, sizeof entities[entityIndex].name, "%s", name);
        snprintf(entities[entityIndex].team, sizeof entities[entityIndex].team, "%s", team);
        entities[entityIndex].positionX = positionX;
        entities[entityIndex].positionY = positionY;
        entities[entityIndex].dimensionX = dimensionX;
        entities[entityIndex].dimensionY = dimensionY;
        entities[entityIndex].R_Color = R_Color;
        entities[entityIndex].G_Color = G_Color;
        entities[entityIndex].B_Color = B_Color;
        entities[entityIndex].Alpha = Alpha;
        entities[entityIndex].collision = collision;
        entities[entityIndex].speed = speed;
        entities[entityIndex].alreadyInQuadrant = 0;
        entities[entityIndex].quadrantOutOfBounds = 0;
        entity_count++;
        return entities[entityIndex].id;
    }

Entity *get_entity(int entityId) {
    if (entityId <= 0 || entityId > entity_count) return NULL;
    return &entities[entityId - 1];
}