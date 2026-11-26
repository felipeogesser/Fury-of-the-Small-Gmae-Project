#include "save.h"
#include "engine_internal.h"
#include "inventory.h"
#include "loot.h"
#include "run.h"
#include "run_stats.h"
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

static void capture_run(SaveRun *run) {

    run->stage = run_current_stage();
    run->seed = run_seed();
    run->general_count = inventory_count();
    for (uint32_t i = 0; i < run->general_count; i++) run->generals[i] = (uint8_t)inventory_type_at(i);
    run->losses = run_stats_losses();
    run->pending_loot = loot_has_pending() ? (uint8_t)(loot_pending_stage() + 1) : 0;
    for (uint32_t i = 0; i < RUN_STAGE_COUNT; i++) run->picks[i] = (uint8_t)run_stats_pick(i);

}

static void restore_inventory(const SaveRun *run) {

    if (run->general_count == 0) {
        inventory_reset();
        return;
    }

    enum GeneralType types[INVENTORY_CAPACITY];
    for (uint32_t i = 0; i < run->general_count; i++) types[i] = (enum GeneralType)run->generals[i];
    inventory_restore(types, run->general_count);

}

void save_load(void) {

    if (is_save_disabled()) return;

    save_slots_ensure_dir();

    char path[SAVE_FILE_PATH_SIZE];
    if (!save_file_path(SAVE_PROFILE_FILE, path, sizeof(path))) return;
    uint32_t version = 0;
    if (!save_file_read(path, SAVE_PROFILE_MAGIC, SAVE_PROFILE_VERSION, &profile_buffer, &version)) return;

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
        .starting_army = (uint8_t)inventory_starting_army()
    };
    capture_run(&session.current);
    session.has_current = save_slots_write(&session.current);

}

void save_write(void) {

    if (is_save_disabled() || !session.has_current) return;

    capture_run(&session.current);
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
    restore_inventory(&run);
    run_restore(true, run.stage, run.seed != 0 ? run.seed : run.created_at);

    enum GeneralType picks[RUN_STAGE_COUNT];
    for (uint32_t i = 0; i < RUN_STAGE_COUNT; i++) picks[i] = (enum GeneralType)run.picks[i];
    run_stats_restore(run.losses, picks);
    if (run.pending_loot > 0) loot_grant(run.pending_loot - 1u);

    return run.stage >= RUN_STAGE_COUNT && !loot_has_pending() ? BATTLEPLAN : STAGE_MAP;

}

enum Scene save_open_latest(void) {

    SaveRun latest = {0};
    if (is_save_disabled() || !save_slots_latest(&latest)) return SCENE_NONE;
    return save_open_slot(latest.id);

}

void save_discard_current(void) {

    if (is_save_disabled() || !session.has_current) return;
    save_slots_delete(session.current.id);
    session.has_current = false;

}

_Bool save_delete_slot(uint32_t id) {

    if (is_save_disabled() || !save_slots_delete(id)) return false;

    if (session.has_current && session.current.id == id) {
        session.has_current = false;
        run_restore(false, 0, 0);
    }
    return true;

}
