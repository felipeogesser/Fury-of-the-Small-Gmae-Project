#include "inventory.h"
#include "inventory_internal.h"
#include "engine_internal.h"
#include "field_entry_types.h"
#include "general_types.h"
#include "json_parser.h"
#include "starting_army.h"

Inventory inventory = {0};

static enum StartingArmy starting_army = STARTING_ARMY_TRAINING;

void inventory_set_starting_army(enum StartingArmy army) {

    starting_army = army;

}

void *inventory_init(void) {

    engine.inventory = &inventory;

    const StartingArmyFile file = starting_army_file(starting_army);
    inventory.general_count = (unsigned short)file.general_count;

    General *general = get_json_file_data(
        file.path,
        inventory.general_count,
        "generals",
        sizeof_General,
        general_field_table,
        22,
        "id",
        "rarity",
        "hp",
        "vigour",
        "base_meele_attack",
        "base_meele_defense",
        "armor",
        "attack_speed",
        "general_type",
        "battalion_type",
        "units_type",
        "animation",
        "sprite_type",
        "sprite",
        "base_strength",
        "range",
        "charge_bonus",
        "is_shielded",
        "is_ranged",
        "is_large",
        "is_armor_piercing",
        "is_anti_large"
    );

    inventory.general = general;

    return general;

}
