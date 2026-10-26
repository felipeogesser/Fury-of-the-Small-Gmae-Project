#include "inventory.h"
#include "inventory_internal.h"
#include "engine_internal.h"
#include "field_entry_types.h"
#include "general_types.h"
#include "json_parser.h"

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
