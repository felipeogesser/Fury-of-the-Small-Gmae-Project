#include "save.h"
#include "engine_internal.h"
#include "inventory.h"
#include "run.h"
#include "save_file.h"
#include "save_slots.h"
#include "tutorial.h"
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define SAVE_PROFILE_FILE "profile.bin"
#define SAVE_PROFILE_MAGIC "CGPF"
#define SAVE_PROFILE_VERSION 1u

typedef struct SaveSession {

    SaveRun current;
    _Bool has_current;

} SaveSession;

static SaveSession session = {0};
static SaveBuffer profile_buffer;

static _Bool is_save_disabled(void) {

    return engine.has_program_been_initialized_with_ai_bypass;

}

static uint64_t now_ms(void) {

    static uint64_t last_stamp = 0;
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    const uint64_t stamp = (uint64_t)now.tv_sec * 1000u + (uint64_t)now.tv_nsec / 1000000u;
    last_stamp = stamp > last_stamp ? stamp : last_stamp + 1;
    return last_stamp;

}

static void write_profile(void) {

    char path[SAVE_FILE_PATH_SIZE];
    if (!save_file_path(SAVE_PROFILE_FILE, path, sizeof(path))) return;

    save_buffer_reset(&profile_buffer);
    save_buffer_put_u8(&profile_buffer, tutorial_is_finished());
    save_file_write(path, SAVE_PROFILE_MAGIC, SAVE_PROFILE_VERSION, &profile_buffer);

}

void save_load(void) {

    if (is_save_disabled()) return;

    save_slots_ensure_dir();

    char path[SAVE_FILE_PATH_SIZE];
    if (!save_file_path(SAVE_PROFILE_FILE, path, sizeof(path))) return;
    if (!save_file_read(path, SAVE_PROFILE_MAGIC, SAVE_PROFILE_VERSION, &profile_buffer)) return;

    const uint8_t is_tutorial_finished = save_buffer_get_u8(&profile_buffer);
    if (!save_buffer_is_consumed(&profile_buffer)) {
        fprintf(stderr, "save: %s has invalid contents\n", path);
        return;
    }
    tutorial_set_finished(is_tutorial_finished != 0);

}

void save_begin_run(void) {

    session.has_current = false;
    if (is_save_disabled() || !tutorial_is_finished()) return;

    write_profile();

    const uint64_t now = now_ms();
    session.current = (SaveRun){
        .id = save_slots_next_id(),
        .created_at = now,
        .last_played_at = now,
        .stage = run_current_stage(),
        .starting_army = (uint8_t)inventory_starting_army()
    };
    session.has_current = save_slots_write(&session.current);

}

void save_write(void) {

    if (is_save_disabled() || !session.has_current) return;

    session.current.stage = run_current_stage();
    session.current.last_played_at = now_ms();
    save_slots_write(&session.current);

}

_Bool save_has_any(void) {

    if (is_save_disabled()) return false;
    SaveRun latest = {0};
    return save_slots_latest(&latest);

}

enum Scene save_open_slot(uint32_t id) {

    if (is_save_disabled()) return SCENE_NONE;

    SaveRun run = {0};
    if (!save_slots_read(id, &run)) return SCENE_NONE;

    run.last_played_at = now_ms();
    save_slots_write(&run);
    session.current = run;
    session.has_current = true;

    inventory_set_starting_army((enum StartingArmy)run.starting_army);

    if (run.stage >= RUN_STAGE_COUNT) {
        run_restore(true, RUN_STAGE_COUNT - 1);
        return BATTLEPLAN;
    }
    run_restore(true, run.stage);
    return STAGE_MAP;

}

enum Scene save_open_latest(void) {

    SaveRun latest = {0};
    if (is_save_disabled() || !save_slots_latest(&latest)) return SCENE_NONE;
    return save_open_slot(latest.id);

}

_Bool save_delete_slot(uint32_t id) {

    if (is_save_disabled() || !save_slots_delete(id)) return false;

    if (session.has_current && session.current.id == id) {
        session.has_current = false;
        run_restore(false, 0);
    }
    return true;

}
