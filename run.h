#ifndef RUN_H
#define RUN_H

#include "shared_memory_control.h"

#define RUN_STAGE_COUNT 10u

void run_new(void);

_Bool run_is_active(void);

unsigned int run_current_stage(void);

_Bool run_is_complete(void);

void run_record_result(enum EpisodeResult result);

_Bool run_take_advanced(void);

#endif
