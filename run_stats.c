#include "run_stats.h"

typedef struct RunStats {

    unsigned int losses;
    enum GeneralType picks[RUN_STAGE_COUNT];

} RunStats;

static RunStats stats = {0};

void run_stats_reset(void) {

    stats = (RunStats){0};

}

void run_stats_restore(unsigned int losses, const enum GeneralType *picks) {

    stats.losses = losses < RUN_LIVES ? losses : RUN_LIVES - 1;
    for (unsigned int i = 0; i < RUN_STAGE_COUNT; i++) stats.picks[i] = picks[i];

}

void run_stats_record_loss(void) {

    stats.losses++;

}

void run_stats_record_pick(unsigned int stage, enum GeneralType type) {

    if (stage < RUN_STAGE_COUNT) stats.picks[stage] = type;

}

unsigned int run_stats_losses(void) {

    return stats.losses;

}

unsigned int run_stats_lives(void) {

    return stats.losses < RUN_LIVES ? RUN_LIVES - stats.losses : 0;

}

enum GeneralType run_stats_pick(unsigned int stage) {

    return stage < RUN_STAGE_COUNT ? stats.picks[stage] : GENERAL_PADDING_FOR_ZERO;

}
