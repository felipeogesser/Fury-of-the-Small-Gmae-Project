#include "entityMaker.h"

int entity_id[MAX_ENTITIES];
int i = 0;
int I = 0;

void make_entities(void) {
    entity_id[I-1] = I = create_entity("bob", "blue", 30.0f, 30.0f, 20.0f, 20.0f, 222, 0, 211, 255, true, 200.0f);
}

void auto_make_entities(int iter) {
    for (i = 0; i < iter; i++) {
        I = create_entity("bob", "red", 30.0f, 30.0f, 20.0f, 20.0f, rand() % 256, rand() % 256, rand() % 256, 255, true, 200.0f);
        entity_id[I-1] = I;
    }
}

void team_blue_entities(int iter) {
    for (i = 0; i < iter; i++) {
        I = create_entity("bob", "blue", 30.0f, 30.0f, 20.0f, 20.0f, 0, 0, 255, 255, true, 200.0f);
        entity_id[I-1] = I;
    }
}

void team_red_entities(int iter) {
    for (i = 0; i < iter; i++) {
        I = create_entity("bob", "red", 30.0f, 30.0f, 20.0f, 20.0f, 255, 0, 0, 255, true, 200.0f);
        entity_id[I-1] = I;
    }
}