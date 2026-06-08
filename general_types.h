#ifndef GENERAL_TYPES_H
#define GENERAL_TYPES_H

enum GeneralType {
    MELEE,
    RANGED,
    SPELLCASTER,
    GENERAL_TYPE_COUNT
};

enum BattalionType {
    INFANTRY,
    CAVALRY,
    SPEARMEN,
    ARCHERS,
    CROSSBOWS,
    WIZARDS,
    HEALERS,
    BATTALION_TYPE_COUNT = 32
};

enum FatigueLevel {
    FRESH,
    ACTIVE,
    WINDED,
    TIRED,
    VERY_TIRED,
    EXHAUSTED
};

typedef struct General General;

extern const unsigned int *const general_battalion_taxonomy_type_masks;

#endif