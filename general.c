#include "general.h"
#include "general_internal.h"
#include "armies_internal.h"
#include "game_state_internal.h"

#define BATTALION_LIST(X) \
    X(INFANTRY,  MELEE) \
    X(CAVALRY,   MELEE) \
    X(SPEARMEN,  MELEE) \
    X(ARCHERS,   RANGED) \
    X(CROSSBOWS, RANGED) \
    X(WIZARDS,   SPELLCASTER) \
    X(HEALERS,   SPELLCASTER)

static unsigned int internal_taxonomy_masks[GENERAL_TYPE_COUNT];

const unsigned int *const general_battalion_taxonomy_type_masks = internal_taxonomy_masks;

void init_general_battalion_taxonomy(void) {
    #define X(name, group) internal_taxonomy_masks[group] |= (1U << name);
        BATTALION_LIST(X)
    #undef X
}

void create_generals(General *general, float ongoing_point_X, float ongoing_point_Y, unsigned int i) {
    general[i].id = i + 1;
    general[i].positionX = ongoing_point_X + 40;
    general[i].positionY = ongoing_point_Y + 60;
    general[i].dimensionX = 8;
    general[i].dimensionY = 8;
}

void update_generals(Armies *armies, GameState *game) {
    
    General *general = armies->army->general;

    for (unsigned int i = 0; i < 6; i++) {
        general[i].positionX += general[i].vectorX * game->delta;
        general[i].positionY += general[i].vectorY * game->delta;
    }

}