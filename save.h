#ifndef SAVE_H
#define SAVE_H

#include "scenes.h"
#include <stdint.h>

void save_load(void);
void save_begin_run(void);
void save_write(void);

_Bool save_has_any(void);
enum Scene save_open_slot(uint32_t id);
enum Scene save_open_latest(void);
_Bool save_delete_slot(uint32_t id);

#endif
