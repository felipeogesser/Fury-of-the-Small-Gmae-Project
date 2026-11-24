#include "game_settings.h"
#include "engine_internal.h"
#include "save_file.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#define SETTINGS_FILE "settings.bin"
#define SETTINGS_MAGIC "CGST"
#define SETTINGS_VERSION 1u

typedef struct SettingDef {

    uint16_t key;
    int32_t default_value;
    int32_t min;
    int32_t max;
    void (*apply)(int value);

} SettingDef;

static void apply_fullscreen(int value);

static const SettingDef setting_defs[SETTING_COUNT] = {
    [SETTING_FULLSCREEN] = {1, 0, 0, 1, apply_fullscreen}
};

static int32_t values[SETTING_COUNT];
static _Bool are_values_ready = false;
static SaveBuffer buffer;

static void apply_fullscreen(int value) {

    if (engine.window == NULL) return;
    SDL_SetWindowFullscreen(engine.window, value ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);

}

static _Bool is_persistence_disabled(void) {

    return engine.has_program_been_initialized_with_ai_bypass;

}

static int32_t clamp_value(enum Setting setting, int32_t value) {

    const SettingDef *def = &setting_defs[setting];
    if (value < def->min) return def->min;
    if (value > def->max) return def->max;
    return value;

}

static void reset_defaults(void) {

    for (unsigned int i = 0; i < SETTING_COUNT; i++) values[i] = setting_defs[i].default_value;
    are_values_ready = true;

}

static void ensure_values(void) {

    if (!are_values_ready) reset_defaults();

}

static _Bool find_setting(uint16_t key, enum Setting *setting) {

    for (unsigned int i = 0; i < SETTING_COUNT; i++) {
        if (setting_defs[i].key != key) continue;
        *setting = (enum Setting)i;
        return true;
    }
    return false;

}

static void write_settings(void) {

    if (is_persistence_disabled()) return;

    char path[SAVE_FILE_PATH_SIZE];
    if (!save_file_path(SETTINGS_FILE, path, sizeof(path))) return;

    save_buffer_reset(&buffer);
    save_buffer_put_u32(&buffer, SETTING_COUNT);
    for (unsigned int i = 0; i < SETTING_COUNT; i++) {
        save_buffer_put_u32(&buffer, setting_defs[i].key);
        save_buffer_put_u32(&buffer, (uint32_t)values[i]);
    }
    save_file_write(path, SETTINGS_MAGIC, SETTINGS_VERSION, &buffer);

}

void game_settings_load(void) {

    reset_defaults();
    if (is_persistence_disabled()) return;

    char path[SAVE_FILE_PATH_SIZE];
    if (!save_file_path(SETTINGS_FILE, path, sizeof(path))) return;
    if (!save_file_read(path, SETTINGS_MAGIC, SETTINGS_VERSION, &buffer)) return;

    int32_t loaded[SETTING_COUNT];
    for (unsigned int i = 0; i < SETTING_COUNT; i++) loaded[i] = values[i];

    const uint32_t count = save_buffer_get_u32(&buffer);
    for (uint32_t i = 0; i < count && buffer.is_valid; i++) {
        const uint32_t key = save_buffer_get_u32(&buffer);
        const int32_t value = (int32_t)save_buffer_get_u32(&buffer);
        enum Setting setting = SETTING_COUNT;
        if (key <= UINT16_MAX && find_setting((uint16_t)key, &setting)) loaded[setting] = clamp_value(setting, value);
    }

    if (!save_buffer_is_consumed(&buffer)) {
        fprintf(stderr, "save: %s has invalid contents\n", path);
        return;
    }
    for (unsigned int i = 0; i < SETTING_COUNT; i++) values[i] = loaded[i];

}

void game_settings_apply_all(void) {

    ensure_values();
    for (unsigned int i = 0; i < SETTING_COUNT; i++) {
        if (setting_defs[i].apply != NULL) setting_defs[i].apply(values[i]);
    }

}

int game_settings_get(enum Setting setting) {

    ensure_values();
    return values[setting];

}

void game_settings_set(enum Setting setting, int value) {

    ensure_values();
    const int32_t clamped = clamp_value(setting, value);
    if (values[setting] == clamped) return;

    values[setting] = clamped;
    if (setting_defs[setting].apply != NULL) setting_defs[setting].apply(clamped);
    write_settings();

}

void game_settings_toggle(enum Setting setting) {

    game_settings_set(setting, game_settings_get(setting) ? 0 : 1);

}
