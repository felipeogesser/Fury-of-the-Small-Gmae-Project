#include "load_armies.h"
#include "animation_internal.h"
#include "armies_internal.h"
#include "battalion_internal.h"
#include "battleplan_internal.h"
#include "engine_internal.h"
#include "game_state_internal.h"
#include "general.h"
#include "general_internal.h"
#include "memory_arena.h"
#include "sprites_internal.h"
#include "unit.h"
#include "unit_internal.h"
//#include "set_armies_position.h"
#include <stddef.h>

// prototypes
static unsigned int count_battalions_in_battleplan(void);
static General *get_general_from_grid(General (*grid)[GRID_DIMENSION_Y], unsigned int *x, unsigned int *y);
static void set_battalion_position(Battalion *battalion, unsigned int idx, unsigned int x, unsigned int y);
static void set_units_sprite_and_animation(General *general, unsigned int battalion_size);

size_t armies_total_memory_size;
static unsigned int armies_count = 1;
static unsigned int battalion_count = 0;
static unsigned int battalion_size = 50;

void init_armies_memory_arena(void) {

    battalion_count = count_battalions_in_battleplan();
    engine.game->unit_created_count = (unsigned int)(battalion_size * battalion_count * armies_count);

    armies_total_memory_size =
        (_Alignof(Armies) - 1) + sizeof(Armies) +
        (_Alignof(Army) - 1) + armies_count * sizeof(Army) +
        (_Alignof(General) - 1) + armies_count * battalion_count * sizeof(General) +
        (_Alignof(Battalion) - 1) + armies_count * battalion_count * sizeof(Battalion) +
        (_Alignof(Unit) - 1) + armies_count * battalion_count * battalion_size * sizeof(Unit);
        
    if (memory_arena_memory_remainder() < armies_total_memory_size) {

        fprintf(stderr, "Armies memory allocation failed. Not enough memory available.\n");
        exit(EXIT_FAILURE);

    }
    
    engine.army_memory_ptr = memory_arena_current_pointer();

    Armies *armies = memory_arena_push(sizeof(Armies), _Alignof(Armies));

    Army *army = memory_arena_push(
        sizeof(Army) * armies_count, _Alignof(Army));

    General *general = memory_arena_push(
        sizeof(General) * armies_count * battalion_count,
        _Alignof(General));

    Battalion *battalions = memory_arena_push(
        sizeof(Battalion) * armies_count * battalion_count,
        _Alignof(Battalion));

    Unit *unit = memory_arena_push(
        sizeof(Unit) * armies_count * battalion_count * battalion_size,
        _Alignof(Unit));

    //
    /*engine.army_memory_ptr = memory_arena_current_pointer();

    char *p = engine.army_memory_ptr;
    
    engine.armies = (Armies *)p;

    Armies *armies = engine.armies;

    p += sizeof(Armies);

    p = (char *)(((uintptr_t)p + _Alignof(Army) - 1) & ~(_Alignof(Army) - 1));
    Army *army = (Army *)p;

    p += armies_count * sizeof(Army);

    p = (char *)(((uintptr_t)p + _Alignof(Battalion) - 1) & ~(_Alignof(Battalion) - 1));
    Battalion *battalions = (Battalion *)p;

    p += armies_count * battalion_count * sizeof(Battalion);

    p = (char *)(((uintptr_t)p + _Alignof(Unit) - 1) & ~(_Alignof(Unit) - 1));
    Unit *unit = (Unit *)p;

    armies->army = army;*/

    engine.armies = armies;
    engine.armies->army = army;
    engine.armies->armies_count = (unsigned int)armies_count;
    engine.armies->generals_screen_width = 24;
    engine.armies->generals_screen_height = 24;
    for (size_t i = 0; i < armies_count; i++) {

        engine.armies->army[i].general = &general[i * battalion_count];
        engine.armies->army[i].battalions = &battalions[i * battalion_count];
        size_t unit_offset_per_army = battalion_count * battalion_size * i;

        for (size_t j = 0; j < battalion_count; j++) {
            engine.armies->army[i].general[j].battalions = &battalions[j + i * battalion_count];
            engine.armies->army[i].battalions[j].general = &general[j + i * battalion_count];
            engine.armies->army[i].battalions[j].unit = &unit[battalion_size * j + unit_offset_per_army];
            engine.armies->army[i].battalions[j].unit_count = (unsigned int)battalion_size;
            engine.armies->army[i].battalions[j].unit_screen_width = 20;
            engine.armies->army[i].battalions[j].unit_screen_height = 20;
        }
    }
    //engine.armies->army->battalions->unit_count = (unsigned int)battalion_size;

}

void load_armies_into_arena(void) {
    
    Army *army = engine.armies->army;
    General (*grid)[GRID_DIMENSION_Y] = engine.battleplan->battleplan_general_placement;
    unsigned int x = 0;
    unsigned int y = 0;
    for (unsigned int i = 0; i < armies_count; i++) {

        Battalion *battalion = army[i].battalions;
        for (unsigned int j = 0; j < battalion_count; j++) {

            // OR logic pode ser substituido por save and restore os ponteiros, para que nao sejam apagados em memcpy
            // OR logic funciona somente porque o source nao possui nenhum ponteiro inicializado, enquanto o target possui
            General *general = get_general_from_grid(grid, &x, &y);
            unsigned char *d = (unsigned char *)battalion[j].general;
            const unsigned char *s = (unsigned char *)general;
            for (size_t k = 0; k < sizeof(General); k++) {
                d[k] |= s[k];
            }
            //memcpy(battalion[j].general, general, sizeof(General));
            set_battalion_position(battalion, j, x, y);

            set_units_sprite_and_animation(battalion[j].general, battalion_size);

        }

    }

}

static General *get_general_from_grid(General (*grid)[GRID_DIMENSION_Y], unsigned int *p_x, unsigned int *p_y) {

    unsigned int x = *p_x;
    unsigned int y = *p_y;

    General general = {0};

    for (; x < GRID_DIMENSION_X; x++) {

        for (; y < GRID_DIMENSION_Y; y++) {
            
            if (memcmp(&grid[x][y], &general, sizeof(General)) != 0) {
                
                *p_x = x;
                *p_y = y;
                return &grid[x][y];

            }

        }

        y = 0;

    }

    fprintf(stderr, "get general from grid could not find a general in the grid\n");
    exit(EXIT_FAILURE);

    return NULL;

}

static void set_battalion_position(Battalion *battalion, unsigned int idx, unsigned int x, unsigned int y) {
    
    //Map *map = get_map(1);
    //signed int map_size_x = map->mapSizeX;
    //signed int map_size_y = map->mapSizeY;

    unsigned int padding_left = 100;
    //unsigned int padding_right = 100;
    unsigned int padding_top = 50;
    //unsigned int padding_bottom = 50;
    unsigned int battalion_width = 50;
    unsigned int battalion_height = 100;
    unsigned int padding_between_battalions_x = 20;
    unsigned int padding_between_battalions_y = 10;

    unsigned int grid_x_relative_to_map = padding_left + x * battalion_width + padding_between_battalions_x;
    unsigned int grid_y_relative_to_map = padding_top + y * battalion_height + padding_between_battalions_y;

    General *general = battalion[idx].general;
    general->positionX = grid_x_relative_to_map + battalion_width - 5;
    general->positionY = grid_y_relative_to_map + battalion_height / 2;

    unsigned int unit_width = 5;
    unsigned int unit_height = 8;
    unsigned int padding_between_units_x = 12;
    unsigned int padding_between_units_y = 14;
    unsigned int id = 0;
    Unit *unit = battalion[idx].unit;
    for (unsigned int i = 0; i < battalion_count; i++) {

        for (unsigned int j = 0; j < battalion_size; j++) {
        
            float pos_x = grid_x_relative_to_map + padding_between_units_x + unit_width * i;
            float pos_y = grid_y_relative_to_map + padding_between_units_y + unit_height * j;
            create_unit(unit, general, pos_x, pos_y, id); // remover create unit para separar responsabilidades. manter somente pos update
            id++;
        }

    }

}

static void set_units_sprite_and_animation(General *general, unsigned int batt_size) {

    Unit *unit = general->battalions->unit;
    for (unsigned int i = 0; i < batt_size; i++) {

        unit[i].sprite.type = general->units_type;
        unit[i].anim.animation = IDLE;
        unit[i].anim.frames_count = engine.sprite_pack->sprite[general->units_type][IDLE].frames_count;
        unit[i].anim.current_frame = (unsigned char)(rand() % unit[i].anim.frames_count);

    }
    
}

void free_army_memory(void) {
    
    memset(engine.army_memory_ptr, 0, armies_total_memory_size);
    engine.army_memory_ptr = NULL;

}

/*void load_armies_into_arena(void) {
    
    engine.armies->armies_count = (unsigned int)armies_count;
    Army *army_ptr = engine.armies->army;
    
    for (unsigned int i = 0; i < armies_count; i++) {

        army_ptr[i].battalion_count = battalion_count;
        army_ptr[i].army_size = battalion_count * battalion_size;
        army_ptr[i].unit_alive = battalion_count * battalion_size;
        
        Battalion *battalions_ptr = army_ptr[i].battalions;

        for (unsigned int j = 0; j < battalion_count; j++) {
            
            unsigned int x = (j + 1) * 2654435761u;

            unsigned char r = (x >>  0) & 255u;
            unsigned char g = (x >>  8) & 255u;
            unsigned char b = (x >> 16) & 255u;

            battalions_ptr[j].unit_count = battalion_size;
            battalions_ptr[j].R_Color = r;
            battalions_ptr[j].G_Color = g;
            battalions_ptr[j].B_Color = b;
            battalions_ptr[j].Alpha = (unsigned char)255;

            Unit *unit_ptr = battalions_ptr[j].unit;
            
            for (unsigned int k = 0; k < battalion_size; k++) {
                
                set_armies_in_the_battlefield(unit_ptr);
            
            }
        }
    }
}*/

static unsigned int count_battalions_in_battleplan(void) {

    unsigned int count = 0;
    General general = {0};

    for (unsigned int i = 0; i < GRID_DIMENSION_X; i++) {

        for (unsigned int j = 0; j < GRID_DIMENSION_Y; j++) {

            if (memcmp(&engine.battleplan->battleplan_general_placement[i][j], &general, sizeof(General)) != 0) {

                count++;

            }

        }

    }

    return count;

}


/*void load_armies_into_arena(void) { //funcao precisa ser melhorada dps, mt desorganizado
    
    Army *army = engine.armies->army;
    
    for (unsigned int i = 0; i < armies_count; i++) {

        army[i].battalion_count = battalion_count;
        army[i].army_size = battalion_count * battalion_size;
        army[i].unit_alive = battalion_count * battalion_size;
    
    }
        
    set_armies_in_the_battlefield(engine.armies);
    
}*/