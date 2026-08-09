#include "type_tables.h"
#include "general_types.h"
#include "json_parser_types.h"
#include "sprites_types.h"
#include <stddef.h>
#include <string.h>

const KindEntry json_kind_table[] = {
    { "general", GENERAL },
    { "unit", UNIT },
    { "item", ITEM },
    { "weapon", WEAPON },
};
const size_t json_kind_table_count = (sizeof(json_kind_table) / sizeof(json_kind_table[0]));

const RarityEntry rarity_table[] = {
    { "common", COMMON },
    { "rare", RARE },
    { "unique", UNIQUE },
};
const size_t rarity_table_count = (sizeof(rarity_table) / sizeof(rarity_table[0]));

const GeneralTypeEntry general_type_table[] = {
    { "melee", MELEE },
    { "ranged", RANGED },
};
const size_t general_type_table_count = (sizeof(general_type_table) / sizeof(general_type_table[0]));

const BattalionTypeEntry battalion_type_table[] = {
    { "infantry", INFANTRY },
    { "cavalry", CAVALRY },
    { "spearmen", SPEARMEN },
    { "archers", ARCHERS },
};
const size_t battalion_type_table_count = (sizeof(battalion_type_table) / sizeof(battalion_type_table[0]));

const SpritesEntry sprites_table[] = {
    { "archer", ARCHER },
    { "lancer", LANCER },
    { "warrior", WARRIOR },
    { "rafa", RAFA },
};
const size_t sprites_table_count = (sizeof(sprites_table) / sizeof(sprites_table[0]));

const TableRef all_tables[] = {
    { "kind",           json_kind_table,       json_kind_table_count,       sizeof(json_kind_table[0]) },
    { "rarity",         rarity_table,           rarity_table_count,          sizeof(rarity_table[0]) },
    { "general_type",   general_type_table,     general_type_table_count,    sizeof(general_type_table[0]) },
    { "battalion_type", battalion_type_table,   battalion_type_table_count,  sizeof(battalion_type_table[0]) },
    { "sprite",         sprites_table,          sprites_table_count,         sizeof(sprites_table[0]) },
};
const size_t all_tables_count = sizeof(all_tables) / sizeof(all_tables[0]);

enum JsonKind string_to_kind(const char *str) {
    for (unsigned int i = 0; i < json_kind_table_count; i++) {
        if (strcmp(str, json_kind_table[i].name) == 0) {
            return json_kind_table[i].value;
        }
    }
    return KIND_UNKNOWN;
}

const char *kind_to_string(enum JsonKind r) {
    for (unsigned int i = 0; i < json_kind_table_count; i++) {
        if (json_kind_table[i].value == r) {
            return json_kind_table[i].name;
        }
    }
    return "kind_unknown";
}

enum Rarity string_to_rarity(const char *str) {
    for (unsigned int i = 0; i < rarity_table_count; i++) {
        if (strcmp(str, rarity_table[i].name) == 0) {
            return rarity_table[i].value;
        }
    }
    return RARITY_UNKNOWN;
}

const char *rarity_to_string(enum Rarity r) {
    for (unsigned int i = 0; i < rarity_table_count; i++) {
        if (rarity_table[i].value == r) {
            return rarity_table[i].name;
        }
    }
    return "rarity_unknown";
}

enum GeneralType string_to_general_type(const char *str) {
    for (unsigned int i = 0; i < general_type_table_count; i++) {
        if (strcmp(str, general_type_table[i].name) == 0) {
            return general_type_table[i].value;
        }
    }
    return GENERAL_TYPE_UNKNOWN;
}

const char *general_type_to_string(enum GeneralType r) {
    for (unsigned int i = 0; i < general_type_table_count; i++) {
        if (general_type_table[i].value == r) {
            return general_type_table[i].name;
        }
    }
    return "general_type_unknown";
}

enum BattalionType string_to_battalion_type(const char *str) {
    for (unsigned int i = 0; i < battalion_type_table_count; i++) {
        if (strcmp(str, battalion_type_table[i].name) == 0) {
            return battalion_type_table[i].value;
        }
    }
    return BATTALION_TYPE_UNKNOWN;
}

const char *battalion_type_to_string(enum BattalionType r) {
    for (unsigned int i = 0; i < battalion_type_table_count; i++) {
        if (battalion_type_table[i].value == r) {
            return battalion_type_table[i].name;
        }
    }
    return "battalion_type_unknown";
}

enum Sprites string_to_sprite(const char *str) {
    for (unsigned int i = 0; i < sprites_table_count; i++) {
        if (strcmp(str, sprites_table[i].name) == 0) {
            return sprites_table[i].value;
        }
    }
    return SPRITE_UNKNOWN;
}

const char *sprite_to_string(enum Sprites r) {
    for (unsigned int i = 0; i < sprites_table_count; i++) {
        if (sprites_table[i].value == r) {
            return sprites_table[i].name;
        }
    }
    return "sprite_unknown";
}
