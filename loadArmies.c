#include "loadArmies.h"
#include "setArmiesPosition.h"

static void *army_memory = NULL;
static Armies *armies = NULL;
static size_t total;

void init_armies_memory_arena(void) {

    size_t battalion_size = 50;
    size_t number_of_battalions = 3;
    size_t number_of_armies = 2;

    total =
        sizeof(Armies) +
        number_of_armies * sizeof(Army) + (alignof(Army) - 1) +
        number_of_armies * number_of_battalions * sizeof(Battalion) + (alignof(Battalion) - 1) +
        number_of_armies * number_of_battalions * battalion_size * sizeof(Entity) + (alignof(Entity) - 1);

    army_memory = calloc(1, total);

    char *p = army_memory;
    
    Armies *armies = (Armies *)p;

    p += sizeof(Armies);

    p = (char*)(((uintptr_t)p + alignof(Army) - 1) & ~(alignof(Army) - 1));
    Army *army = (Army *)p;

    p += number_of_armies * sizeof(Army);

    p = (char*)(((uintptr_t)p + alignof(Battalion) - 1) & ~(alignof(Battalion) - 1));
    Battalion *battalions = (Battalion *)p;

    p += number_of_armies * number_of_battalions * sizeof(Battalion);

    p = (char*)(((uintptr_t)p + alignof(Entity) - 1) & ~(alignof(Entity) - 1));
    Entity *entities = (Entity *)p;

    armies->army = army;
    for (size_t i = 0; i < number_of_armies; i++) {
        army[i].battalions = &battalions[i * number_of_battalions];
        size_t entity_offset_per_army = number_of_battalions * battalion_size * i;
        for (size_t j = 0; j < number_of_battalions; j++) {
                army[i].battalions[j].entities = &entities[battalion_size * j + entity_offset_per_army];
        }
    }
}

void load_armies_into_arena(void) {
    Army *army_ptr = armies->army;
    for (unsigned int i = 0; i < number_of_armies; i++) {
        Battalion *battalions_ptr = army_ptr[i].battalions;
        for (unsigned int j = 0; j < number_of_battalions; j++) {
            Entity *entities_ptr = battalions_ptr[j].entities;
            for (unsigned int k = 0; k < battalion_size; k++) {
                set_armies_in_the_battlefield(entities_ptr);
            }
        }
    }
}

void free_army_memory(void) {
    free(army_memory);
    army_memory = NULL;
}