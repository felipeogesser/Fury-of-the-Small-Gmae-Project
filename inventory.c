#include "inventory.h"
#include "inventory_internal.h"
#include "deployment_loader.h"
#include "engine_internal.h"
#include "field_entry_types.h"
#include "general_internal.h"
#include "json_parser.h"
#include "starting_army.h"
#include <stdbool.h>
#include <stdlib.h>

#define INVENTORY_VIGOUR 100

typedef struct OwnedGenerals {

    enum GeneralType types[INVENTORY_CAPACITY];
    unsigned int count;

} OwnedGenerals;

Inventory inventory = {0};

static enum StartingArmy starting_army = STARTING_ARMY_TRAINING;
static OwnedGenerals owned = {0};

static _Bool is_valid_type(enum GeneralType type) {

    return type > GENERAL_PADDING_FOR_ZERO && type < GENERAL_UNIQUE;

}

void inventory_set_starting_army(enum StartingArmy army) {

    starting_army = army;

}

enum StartingArmy inventory_starting_army(void) {

    return starting_army;

}

static General *load_starting_army_file(const StartingArmyFile *file) {

    return get_json_file_data(
        file->path,
        (unsigned short)file->general_count,
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

}

void inventory_reset(void) {

    owned.count = 0;

    const StartingArmyFile file = starting_army_file(starting_army);
    General *generals = load_starting_army_file(&file);
    if (generals == NULL) return;

    for (unsigned int i = 0; i < file.general_count; i++) inventory_add(generals[i].general_type);
    free(generals);

}

_Bool inventory_add(enum GeneralType type) {

    if (!is_valid_type(type) || owned.count >= INVENTORY_CAPACITY) return false;
    owned.types[owned.count++] = type;
    return true;

}

void inventory_restore(const enum GeneralType *types, unsigned int count) {

    owned.count = 0;
    for (unsigned int i = 0; i < count; i++) inventory_add(types[i]);

}

unsigned int inventory_count(void) {

    return owned.count;

}

enum GeneralType inventory_type_at(unsigned int index) {

    return index < owned.count ? owned.types[index] : GENERAL_TYPE_UNKNOWN;

}

void *inventory_init(void) {

    engine.inventory = &inventory;
    if (owned.count == 0) inventory_reset();

    free(inventory.general);
    inventory.general = calloc(owned.count > 0 ? owned.count : 1, sizeof(General));
    inventory.general_count = 0;
    if (inventory.general == NULL) return NULL;

    for (unsigned int i = 0; i < owned.count; i++) {
        General general = convert_spreadsheet_general_to_general(owned.types[i]);
        general.id = i + 1;
        general.rarity = COMMON;
        general.vigour = INVENTORY_VIGOUR;
        inventory.general[i] = general;
    }
    inventory.general_count = (unsigned short)owned.count;

    return inventory.general;

}
