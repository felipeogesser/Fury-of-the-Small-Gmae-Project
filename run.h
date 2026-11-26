#ifndef RUN_H
#define RUN_H

#include "shared_memory_control.h"
#include <stdint.h>

#define RUN_STAGE_COUNT 10u

void run_new(void);

void run_restore(_Bool is_active, unsigned int current_stage, uint64_t seed);

_Bool run_is_active(void);

unsigned int run_current_stage(void);

uint64_t run_seed(void);

_Bool run_is_complete(void);

_Bool run_is_over(void);

void run_end(void);

void run_record_result(enum EpisodeResult result);

_Bool run_take_advanced(void);

_Bool run_take_lost(void);

#endif
