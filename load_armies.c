#include "load_armies.h"
#include "animation_internal.h"
#include "armies_internal.h"
#include "battalion.h"
#include "battalion_internal.h"
#include "battleplan_battlefield_handoff.h"
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

// private prototypes
static void link_army_memory_hierarchy(
    Armies *armies, Army *army, General *general, Battalion *battalions, Unit *unit,
    unsigned int armies_count, unsigned int battalion_count, unsigned int battalion_size);

size_t armies_total_memory_size = 0;

void init_armies_memory_arena(unsigned int armies_count, unsigned int battalion_count, unsigned int battalion_size) {

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

    link_army_memory_hierarchy(
        armies, army, general, battalions, unit,
        armies_count, battalion_count, battalion_size);

}

void load_armies_into_arena(GeneralPayload *general_payload, unsigned int armies_count, unsigned int battalion_count, unsigned int battalion_size) {
    
    Army *army = engine.armies->army;
    for (unsigned int i = 0; i < armies_count; i++) {

        Battalion *battalion = army[i].battalions;
        General *general = army[i].general;
        GeneralAndPosition *general_and_pos = general_payload->general_and_pos;
        for (unsigned int j = 0; j < battalion_count; j++) {

            init_battalion(&battalion[j], battalion_size);

            init_general(&general[j], general_and_pos[j].general, general_and_pos[j].pos.x, general_and_pos[j].pos.y);

            init_units(&battalion[j], general_and_pos[j].pos.x, general_and_pos[j].pos.y);

        }

    }

}

static void link_army_memory_hierarchy(
    Armies *armies, Army *army, General *general, Battalion *battalions, Unit *unit,
    unsigned int armies_count, unsigned int battalion_count, unsigned int battalion_size) {

    engine.armies = armies;
    engine.armies->army = army;
    engine.armies->army->battalion_count = battalion_count;
    engine.armies->army->general_count = battalion_count;
    engine.armies->armies_count = armies_count;
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

} // future refactor: maintain linking only and put other assignments into obj inits

void free_army_memory(void) {
    
    memset(engine.army_memory_ptr, 0, armies_total_memory_size);
    engine.army_memory_ptr = NULL;

}
