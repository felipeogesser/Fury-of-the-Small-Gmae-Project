#include "stage_army.h"
#include <stdbool.h>

#define STAGE_ARMY_FRONT (GRID_DIMENSION_X - 1)
#define STAGE_ARMY_SALT_LAYOUT 0x5354414745ull
#define STAGE_ARMY_SALT_LOOT 0x4C4F4F54ull

enum StageRole {
    STAGE_ROLE_INFANTRY,
    STAGE_ROLE_RANGED,
    STAGE_ROLE_CAVALRY
};

typedef struct StageRng {

    uint64_t state;

} StageRng;

typedef struct StageCell {

    unsigned int x;
    unsigned int y;

} StageCell;

typedef struct StageWeight {

    enum GeneralType type;
    uint32_t weight;

} StageWeight;

static const StageWeight composition[] = {
    {GENERAL_GUARDSMEN, 3},
    {GENERAL_SWORDSMEN, 3},
    {GENERAL_TEMPLAR, 2},
    {GENERAL_CAVALRY, 2},
    {GENERAL_ARCHERS, 2}
};

static const StageWeight loot_table[] = {
    {GENERAL_GUARDSMEN, 1},
    {GENERAL_SWORDSMEN, 1},
    {GENERAL_TEMPLAR, 1},
    {GENERAL_CAVALRY, 1},
    {GENERAL_ARCHERS, 1}
};

static const unsigned int rows_center_out[GRID_DIMENSION_Y] = {2, 3, 1, 4, 0, 5};

static uint64_t next_u64(StageRng *rng) {

    uint64_t z = (rng->state += 0x9E3779B97F4A7C15ull);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
    return z ^ (z >> 31);

}

static StageRng stage_rng(uint64_t seed, unsigned int stage, uint64_t salt) {

    StageRng rng = {seed ^ salt ^ ((uint64_t)stage << 32)};
    next_u64(&rng);
    return rng;

}

static enum GeneralType pick_weighted(StageRng *rng, const StageWeight *table, unsigned int count) {

    uint32_t total = 0;
    for (unsigned int i = 0; i < count; i++) total += table[i].weight;

    uint32_t roll = (uint32_t)(next_u64(rng) % total);
    for (unsigned int i = 0; i < count; i++) {
        if (roll < table[i].weight) return table[i].type;
        roll -= table[i].weight;
    }
    return table[0].type;

}

static enum StageRole role_of(enum GeneralType type) {

    if (type == GENERAL_ARCHERS) return STAGE_ROLE_RANGED;
    if (type == GENERAL_CAVALRY) return STAGE_ROLE_CAVALRY;
    return STAGE_ROLE_INFANTRY;

}

static unsigned int add_column(StageCell *cells, unsigned int count, unsigned int x) {

    for (unsigned int i = 0; i < GRID_DIMENSION_Y; i++) cells[count++] = (StageCell){x, rows_center_out[i]};
    return count;

}

static unsigned int add_flanks(StageCell *cells, unsigned int count, unsigned int x) {

    cells[count++] = (StageCell){x, 0};
    cells[count++] = (StageCell){x, GRID_DIMENSION_Y - 1};
    return count;

}

static unsigned int preferred_cells(enum StageRole role, StageCell *cells) {

    unsigned int count = 0;

    if (role == STAGE_ROLE_INFANTRY) {
        for (int x = STAGE_ARMY_FRONT; x >= 0; x--) count = add_column(cells, count, (unsigned int)x);
        return count;
    }

    if (role == STAGE_ROLE_RANGED) {
        count = add_column(cells, count, STAGE_ARMY_FRONT - 2);
        count = add_column(cells, count, STAGE_ARMY_FRONT - 3);
        count = add_column(cells, count, STAGE_ARMY_FRONT - 1);
        count = add_column(cells, count, STAGE_ARMY_FRONT - 4);
        count = add_column(cells, count, STAGE_ARMY_FRONT);
        return count;
    }

    for (int x = STAGE_ARMY_FRONT; x >= 0; x--) count = add_flanks(cells, count, (unsigned int)x);
    count = add_column(cells, count, STAGE_ARMY_FRONT - 3);
    count = add_column(cells, count, STAGE_ARMY_FRONT - 2);
    return count;

}

static _Bool place(unsigned int (*layout)[GRID_DIMENSION_Y], enum GeneralType type) {

    StageCell cells[GRID_DIMENSION_X * GRID_DIMENSION_Y * 3];
    const unsigned int count = preferred_cells(role_of(type), cells);

    for (unsigned int i = 0; i < count; i++) {
        if (layout[cells[i].x][cells[i].y] != 0) continue;
        layout[cells[i].x][cells[i].y] = (unsigned int)type;
        return true;
    }

    for (unsigned int x = 0; x < GRID_DIMENSION_X; x++) {
        for (unsigned int y = 0; y < GRID_DIMENSION_Y; y++) {
            if (layout[x][y] != 0) continue;
            layout[x][y] = (unsigned int)type;
            return true;
        }
    }
    return false;

}

unsigned int stage_army_size(unsigned int stage) {

    const unsigned int size = STAGE_ARMY_BASE_SIZE + stage;
    return size > STAGE_ARMY_MAX_SIZE ? STAGE_ARMY_MAX_SIZE : size;

}

void stage_army_layout(unsigned int stage, uint64_t seed, unsigned int (*layout)[GRID_DIMENSION_Y]) {

    for (unsigned int x = 0; x < GRID_DIMENSION_X; x++) {
        for (unsigned int y = 0; y < GRID_DIMENSION_Y; y++) layout[x][y] = 0;
    }

    StageRng rng = stage_rng(seed, stage, STAGE_ARMY_SALT_LAYOUT);
    const unsigned int count = stage_army_size(stage);
    const unsigned int kinds = sizeof(composition) / sizeof(composition[0]);

    enum GeneralType picks[STAGE_ARMY_MAX_SIZE];
    for (unsigned int i = 0; i < count; i++) picks[i] = pick_weighted(&rng, composition, kinds);

    for (enum StageRole role = STAGE_ROLE_INFANTRY; role <= STAGE_ROLE_CAVALRY; role++) {
        for (unsigned int i = 0; i < count; i++) {
            if (role_of(picks[i]) == role) place(layout, picks[i]);
        }
    }

}

enum GeneralType stage_army_loot(unsigned int stage, uint64_t seed) {

    StageRng rng = stage_rng(seed, stage, STAGE_ARMY_SALT_LOOT);
    return pick_weighted(&rng, loot_table, sizeof(loot_table) / sizeof(loot_table[0]));

}
