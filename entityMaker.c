#include "entityMaker.h"

int entity_id[MAX_ENTITIES];

void make_entities(void) {
    entity_id[0] = create_entity("bob", 30.0f, 30.0f, 20.0f, 20.0f, 222, 0, 211, 255, true, 200.0f);
}