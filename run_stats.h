#ifndef RUN_STATS_H
#define RUN_STATS_H

#include "general_types.h"
#include "run.h"

void run_stats_reset(void);
void run_stats_restore(unsigned int losses, const enum GeneralType *picks);

void run_stats_record_loss(void);
void run_stats_record_pick(unsigned int stage, enum GeneralType type);

unsigned int run_stats_losses(void);
enum GeneralType run_stats_pick(unsigned int stage);

#endif
