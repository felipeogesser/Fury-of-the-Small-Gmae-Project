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

typedef struct General General;

extern const unsigned int *const general_battalion_taxonomy_type_masks;

#endif