#include "loadArmies.h"
#include "armies.h"
#include "battalion.h"
#include "engine.h"
#include "entities.h"
#include "memory_arena.h"
#include "setArmiesPosition.h"

size_t armies_total_memory_size;
unsigned int  number_of_armies = 2;
unsigned int number_of_battalions = 3;
unsigned int  battalion_size = 50;

void init_armies_memory_arena(void) {

    engine.game->entities_created_count = (unsigned int)(battalion_size * number_of_battalions * number_of_armies);

    armies_total_memory_size =
        (_Alignof(Armies) - 1) + sizeof(Armies) +
        (_Alignof(Army) - 1) + number_of_armies * sizeof(Army) +
        (_Alignof(Battalion) - 1) + number_of_armies * number_of_battalions * sizeof(Battalion) +
        (_Alignof(Entity) - 1) + number_of_armies * number_of_battalions * battalion_size * sizeof(Entity);
        
    if (memory_arena_memory_remainder() < armies_total_memory_size) {

        fprintf(stderr, "Armies memory allocation failed. Not enough memory available.\n");
        exit(EXIT_FAILURE);

    }
    
    engine.army_memory_ptr = memory_arena_current_pointer();

    Armies *armies = memory_arena_push(sizeof(Armies), _Alignof(Armies));

    Army *army = memory_arena_push(
        sizeof(Army) * number_of_armies, _Alignof(Army));

    Battalion *battalions = memory_arena_push(
        sizeof(Battalion) * number_of_armies * number_of_battalions,
        _Alignof(Battalion));

    Entity *entities = memory_arena_push(
        sizeof(Entity) * number_of_armies * number_of_battalions * battalion_size,
        _Alignof(Entity));

    //
    /*engine.army_memory_ptr = memory_arena_current_pointer();

    char *p = engine.army_memory_ptr;
    
    engine.armies = (Armies *)p;

    Armies *armies = engine.armies;

    p += sizeof(Armies);

    p = (char *)(((uintptr_t)p + _Alignof(Army) - 1) & ~(_Alignof(Army) - 1));
    Army *army = (Army *)p;

    p += number_of_armies * sizeof(Army);

    p = (char *)(((uintptr_t)p + _Alignof(Battalion) - 1) & ~(_Alignof(Battalion) - 1));
    Battalion *battalions = (Battalion *)p;

    p += number_of_armies * number_of_battalions * sizeof(Battalion);

    p = (char *)(((uintptr_t)p + _Alignof(Entity) - 1) & ~(_Alignof(Entity) - 1));
    Entity *entities = (Entity *)p;

    armies->army = army;*/

    engine.armies = armies;
    engine.armies->army = army;
    for (size_t i = 0; i < number_of_armies; i++) {

        engine.armies->army[i].battalions = &battalions[i * number_of_battalions];
        size_t entity_offset_per_army = number_of_battalions * battalion_size * i;
        
        for (size_t j = 0; j < number_of_battalions; j++) {
            
            engine.armies->army[i].battalions[j].entities = &entities[battalion_size * j + entity_offset_per_army];
            engine.armies->army[i].battalions[j].entities_count = (unsigned int)battalion_size;
        
        }
    }
    //engine.armies->army->battalions->entities_count = (unsigned int)battalion_size;

}

void load_armies_into_arena(void) { //funcao precisa ser melhorada dps, mt desorganizado
    
    engine.armies->number_of_armies = (unsigned int)number_of_armies;
    Army *army = engine.armies->army;
    
    for (unsigned int i = 0; i < number_of_armies; i++) {

        army[i].battalion_count = number_of_battalions;
        army[i].army_size = number_of_battalions * battalion_size;
        army[i].entities_alive = number_of_battalions * battalion_size;
    
    }
        
    set_armies_in_the_battlefield(engine.armies);
    
}

void free_army_memory(void) {
    
    memset(engine.army_memory_ptr, 0, armies_total_memory_size);
    engine.army_memory_ptr = NULL;

}

/*void load_armies_into_arena(void) {
    
    engine.armies->number_of_armies = (unsigned int)number_of_armies;
    Army *army_ptr = engine.armies->army;
    
    for (unsigned int i = 0; i < number_of_armies; i++) {

        army_ptr[i].battalion_count = number_of_battalions;
        army_ptr[i].army_size = number_of_battalions * battalion_size;
        army_ptr[i].entities_alive = number_of_battalions * battalion_size;
        
        Battalion *battalions_ptr = army_ptr[i].battalions;

        for (unsigned int j = 0; j < number_of_battalions; j++) {
            
            unsigned int x = (j + 1) * 2654435761u;

            unsigned char r = (x >>  0) & 255u;
            unsigned char g = (x >>  8) & 255u;
            unsigned char b = (x >> 16) & 255u;

            battalions_ptr[j].entities_count = battalion_size;
            battalions_ptr[j].R_Color = r;
            battalions_ptr[j].G_Color = g;
            battalions_ptr[j].B_Color = b;
            battalions_ptr[j].Alpha = (unsigned char)255;

            Entity *entities_ptr = battalions_ptr[j].entities;
            
            for (unsigned int k = 0; k < battalion_size; k++) {
                
                set_armies_in_the_battlefield(entities_ptr);
            
            }
        }
    }
}*/