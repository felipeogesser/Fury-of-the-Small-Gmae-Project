#include "entityMaker.h"

int *entity_id = NULL;

void init_entity_id(void)
{
    entity_id = calloc(MAX_ENTITIES, sizeof *entity_id);
}

int i = 0;
int I = 0;

void make_entities(void) {
    I = create_entity("bob", "blue", 134.0f, 206.0f, 20.0f, 20.0f, 222, 0, 211, 255, true, 200.0f);
    entity_id[I-1] = I;
    I = create_entity("bob", "blue", 302.0f, 135.0f, 20.0f, 20.0f, 222, 0, 211, 255, true, 200.0f);
    entity_id[I-1] = I;
}

void auto_make_entities(int iter) {
    for (i = 0; i < iter; i++) {
        I = create_entity("bob", "red", rand() % 770, rand() % 570, 20.0f, 20.0f, rand() % 256, rand() % 256, rand() % 256, 255, true, 200.0f);
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
/*
void create_units(signed int ongoing_point_X, signed int ongoing_point_Y) {
    create_entity()
}*/