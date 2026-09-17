#include "inventory.h"
#include "inventory_internal.h"
#include "engine_internal.h"
#include "field_entry.h"
#include "general_internal.h"
#include "json_parser.h"
#include "memory_arena.h"
#include "sprites_internal.h"

Inventory inventory = {0};

void *inventory_init(void) {

    engine.inventory = &inventory;

    inventory.general_count = 9;

    General *general = get_json_file_data(
        "inventory.json",
        inventory.general_count,
        "generals",
        sizeof_General,
        general_field_table,
        13,
        "id",
        "rarity",
        "hp",
        "vigour",
        "attack",
        "defense",
        "evasion",
        "attack_speed",
        "general_type",
        "battalion_type",
        "units_type",
        "anim",
        "sprite"
    );

    inventory.general = general;

    return general;

}
