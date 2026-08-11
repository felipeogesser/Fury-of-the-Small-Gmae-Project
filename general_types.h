#ifndef GENERAL_TYPES_H
#define GENERAL_TYPES_H

enum GeneralType {
    GENERAL_TYPE_UNKNOWN = -1,
    MELEE,
    RANGED,
    GENERAL_TYPE_COUNT
};

enum BattalionType {
    BATTALION_TYPE_UNKNOWN = -1,
    INFANTRY,
    CAVALRY,
    SPEARMEN,
    ARCHERS,
    BATTALION_TYPE_COUNT = 32
};

enum FatigueLevel {
    FRESH = 0,
    ACTIVE,
    WINDED,
    TIRED,
    VERY_TIRED,
    EXHAUSTED
};

enum Rarity {
    RARITY_UNKNOWN = -1,
    UNIQUE,
    RARE,
    COMMON,
    RARITY_TYPES_COUNT
};

enum GeneralFields {
    G_FIELD_UNKNOWN = -1,
    G_ANIM_FIELD,
    G_SPRITE_FIELD,
    G_ID_FIELD,
    G_RARITY_FIELD,
    G_HP_FIELD,
    G_VIGOUR_FIELD,
    G_ATTACK_FIELD,
    G_DEFENSE_FIELD,
    G_EVASION_FIELD,
    G_ATTACK_SPEED_FIELD,
    G_GENERAL_TYPE_FIELD,
    G_FATIGUE_LEVEL_FIELD,
    G_BATTALION_TYPE_FIELD,
    G_BATTALIONS_FIELD,
    G_UNITS_TYPE_FIELD
};

typedef struct General General;

extern const unsigned int *const general_battalion_taxonomy_type_masks;

#endif