#ifndef SAVE_SLOTS_H
#define SAVE_SLOTS_H

#include <stddef.h>
#include <stdint.h>

typedef struct SaveRun {

    uint32_t id;
    uint64_t created_at;
    uint64_t last_played_at;
    uint32_t stage;
    uint8_t starting_army;

} SaveRun;

typedef struct SaveSlotList {

    SaveRun *runs;
    size_t count;

} SaveSlotList;

_Bool save_slots_ensure_dir(void);
_Bool save_slots_read(uint32_t id, SaveRun *run);
_Bool save_slots_write(const SaveRun *run);
_Bool save_slots_delete(uint32_t id);

SaveSlotList save_slots_list(void);
void save_slots_free(SaveSlotList *list);
_Bool save_slots_latest(SaveRun *run);
uint32_t save_slots_next_id(void);

#endif
