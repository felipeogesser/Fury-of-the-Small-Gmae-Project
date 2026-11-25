#include "save_slots.h"
#include "run.h"
#include "save_file.h"
#include "starting_army.h"
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SAVE_SLOTS_DIR "saves"
#define SAVE_SLOT_MAGIC "CGRN"
#define SAVE_SLOT_VERSION 2u

typedef void (*SlotVisitor)(uint32_t id, void *context);

static SaveBuffer buffer;

static _Bool slot_path(uint32_t id, char *out, size_t out_size) {

    char relative[64];
    snprintf(relative, sizeof(relative), SAVE_SLOTS_DIR "/run_%u.bin", (unsigned int)id);
    return save_file_path(relative, out, out_size);

}

static _Bool parse_slot_name(const char *name, uint32_t *id) {

    unsigned int value = 0;
    int consumed = 0;
    if (sscanf(name, "run_%u.bin%n", &value, &consumed) != 1) return false;
    if (consumed <= 0 || name[consumed] != '\0' || value == 0) return false;

    char canonical[64];
    snprintf(canonical, sizeof(canonical), "run_%u.bin", value);
    if (strcmp(canonical, name) != 0) return false;

    *id = (uint32_t)value;
    return true;

}

static void visit_slots(SlotVisitor visitor, void *context) {

    char dir_path[SAVE_FILE_PATH_SIZE];
    if (!save_file_path(SAVE_SLOTS_DIR, dir_path, sizeof(dir_path))) return;

    DIR *dir = opendir(dir_path);
    if (dir == NULL) return;

    const struct dirent *entry = NULL;
    while ((entry = readdir(dir)) != NULL) {
        uint32_t id = 0;
        if (parse_slot_name(entry->d_name, &id)) visitor(id, context);
    }
    closedir(dir);

}

_Bool save_slots_ensure_dir(void) {

    return save_file_ensure_dir(SAVE_SLOTS_DIR);

}

_Bool save_slots_read(uint32_t id, SaveRun *run) {

    char path[SAVE_FILE_PATH_SIZE];
    if (!slot_path(id, path, sizeof(path))) return false;
    uint32_t version = 0;
    if (!save_file_read(path, SAVE_SLOT_MAGIC, SAVE_SLOT_VERSION, &buffer, &version)) return false;

    *run = (SaveRun){0};
    run->id = id;
    run->created_at = save_buffer_get_u64(&buffer);
    run->last_played_at = save_buffer_get_u64(&buffer);
    run->stage = save_buffer_get_u32(&buffer);
    run->starting_army = save_buffer_get_u8(&buffer);

    _Bool are_generals_valid = true;
    if (version >= 2) {
        run->seed = save_buffer_get_u64(&buffer);
        run->general_count = save_buffer_get_u32(&buffer);
        are_generals_valid = run->general_count <= INVENTORY_CAPACITY;
        for (uint32_t i = 0; are_generals_valid && i < run->general_count; i++) {
            run->generals[i] = save_buffer_get_u8(&buffer);
            are_generals_valid = run->generals[i] > GENERAL_PADDING_FOR_ZERO && run->generals[i] < GENERAL_UNIQUE;
        }
    }

    const _Bool is_valid = are_generals_valid && save_buffer_is_consumed(&buffer) && run->stage <= RUN_STAGE_COUNT && run->starting_army < STARTING_ARMY_COUNT;
    if (!is_valid) fprintf(stderr, "save: %s has invalid contents\n", path);
    return is_valid;

}

_Bool save_slots_write(const SaveRun *run) {

    char path[SAVE_FILE_PATH_SIZE];
    if (!save_slots_ensure_dir() || !slot_path(run->id, path, sizeof(path))) return false;

    save_buffer_reset(&buffer);
    save_buffer_put_u64(&buffer, run->created_at);
    save_buffer_put_u64(&buffer, run->last_played_at);
    save_buffer_put_u32(&buffer, run->stage);
    save_buffer_put_u8(&buffer, run->starting_army);
    save_buffer_put_u64(&buffer, run->seed);
    save_buffer_put_u32(&buffer, run->general_count);
    for (uint32_t i = 0; i < run->general_count && i < INVENTORY_CAPACITY; i++) save_buffer_put_u8(&buffer, run->generals[i]);
    return save_file_write(path, SAVE_SLOT_MAGIC, SAVE_SLOT_VERSION, &buffer);

}

_Bool save_slots_delete(uint32_t id) {

    char path[SAVE_FILE_PATH_SIZE];
    if (!slot_path(id, path, sizeof(path))) return false;
    if (remove(path) == 0) return true;
    perror("save: delete failed");
    return false;

}

static void collect_run(uint32_t id, void *context) {

    SaveSlotList *list = context;
    SaveRun run = {0};
    if (!save_slots_read(id, &run)) return;

    SaveRun *grown = realloc(list->runs, (list->count + 1) * sizeof(SaveRun));
    if (grown == NULL) return;
    list->runs = grown;
    list->runs[list->count++] = run;

}

static int compare_recent_first(const void *a, const void *b) {

    const SaveRun *left = a;
    const SaveRun *right = b;
    if (left->last_played_at != right->last_played_at) return left->last_played_at > right->last_played_at ? -1 : 1;
    if (left->id != right->id) return left->id > right->id ? -1 : 1;
    return 0;

}

SaveSlotList save_slots_list(void) {

    SaveSlotList list = {NULL, 0};
    visit_slots(collect_run, &list);
    if (list.count > 1) qsort(list.runs, list.count, sizeof(SaveRun), compare_recent_first);
    return list;

}

void save_slots_free(SaveSlotList *list) {

    free(list->runs);
    list->runs = NULL;
    list->count = 0;

}

_Bool save_slots_latest(SaveRun *run) {

    SaveSlotList list = save_slots_list();
    const _Bool has_any = list.count > 0;
    if (has_any) *run = list.runs[0];
    save_slots_free(&list);
    return has_any;

}

static void track_max_id(uint32_t id, void *context) {

    uint32_t *max_id = context;
    if (id > *max_id) *max_id = id;

}

uint32_t save_slots_next_id(void) {

    uint32_t max_id = 0;
    visit_slots(track_max_id, &max_id);
    return max_id + 1;

}
