#include "load_armies.h"
#include "animation_internal.h"
#include "armies_internal.h"
#include "battalion.h"
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
#include <stddef.h>
#include <math.h>

// prototypes
static void link_army_memory_hierarchy(Armies *armies, Army *army, General *general, Battalion *battalions, Unit *unit);
static unsigned int count_battalions_in_battleplan(void);
static General *get_general_from_grid(General (*grid)[GRID_DIMENSION_Y], unsigned int *x, unsigned int *y);

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

    link_army_memory_hierarchy(armies, army, general, battalions, unit);

}

void load_armies_into_arena(void) {
    
    Army *army = engine.armies->army;
    General (*grid)[GRID_DIMENSION_Y] = engine.battleplan->battleplan_general_placement;
    unsigned int x = 0;
    unsigned int y = 0;
    for (unsigned int i = 0; i < armies_count; i++) {

        Battalion *battalion = army[i].battalions;
        for (unsigned int j = 0; j < battalion_count; j++) {

            General *general = get_general_from_grid(grid, &x, &y);

            init_battalion(&battalion[j], battalion_size, x, y);

            init_generals(&battalion[j], general);
                // personal thought comment:
                // because both battalion and general point to
                // one another, the args are quite ambiguous.
                // the same is applied to other functions.

            init_units(&battalion[j]);

        }

    }

}

static void link_army_memory_hierarchy(Armies *armies, Army *army, General *general, Battalion *battalions, Unit *unit) {

    engine.armies = armies;
    engine.armies->army = army;
    engine.armies->army->battalion_count = battalion_count;
    engine.armies->army->general_count = battalion_count;
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

void free_army_memory(void) {
    
    memset(engine.army_memory_ptr, 0, armies_total_memory_size);
    engine.army_memory_ptr = NULL;

}

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
