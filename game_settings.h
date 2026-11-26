#ifndef GAME_SETTINGS_H
#define GAME_SETTINGS_H

enum Setting {
    SETTING_FULLSCREEN,
    SETTING_COUNT
};

void game_settings_load(void);
void game_settings_apply_all(void);

int game_settings_get(enum Setting setting);
void game_settings_set(enum Setting setting, int value);
void game_settings_toggle(enum Setting setting);

#endif
