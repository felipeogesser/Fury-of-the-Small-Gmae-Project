#include "battlefield_pick.h"
#include "camera.h"
#include <SDL2/SDL.h>
#include <limits.h>
#include <stdbool.h>

#define PICK_UNIT_BODY_X 28
#define PICK_UNIT_BODY_Y 12
#define PICK_UNIT_BODY_W 44
#define PICK_UNIT_BODY_H 52
#define PICK_GENERAL_BODY_X 10
#define PICK_GENERAL_BODY_Y (-40)
#define PICK_GENERAL_BODY_W 80
#define PICK_GENERAL_BODY_H 104

typedef struct PickResult {

    long best_distance;
    _Bool is_enemy;
    unsigned int index;

} PickResult;

static void consider(PickResult *result, float world_x, float world_y, SDL_Rect body, int screen_x, int screen_y, _Bool is_enemy, unsigned int battalion) {

    SDL_Rect rect = {(int)world_x + body.x, (int)world_y + body.y, body.w, body.h};
    camera_world_to_screen(&rect);
    if (screen_x < rect.x || screen_x >= rect.x + rect.w || screen_y < rect.y || screen_y >= rect.y + rect.h) return;

    const long dx = screen_x - (rect.x + rect.w / 2);
    const long dy = screen_y - (rect.y + rect.h / 2);
    const long distance = dx * dx + dy * dy;
    if (distance >= result->best_distance) return;

    result->best_distance = distance;
    result->is_enemy = is_enemy;
    result->index = battalion;

}

static void scan_army(PickResult *result, const Table *tables, _Bool is_enemy, unsigned int battalion_count, unsigned int battalion_size, int screen_x, int screen_y) {

    const UnitDynamicTable *units = &tables->unit_tables.friendly_dynamic;
    const GeneralDynamicTable *generals = &tables->general_tables.friendly_dynamic;
    const SDL_Rect unit_body = {PICK_UNIT_BODY_X, PICK_UNIT_BODY_Y, PICK_UNIT_BODY_W, PICK_UNIT_BODY_H};
    const SDL_Rect general_body = {PICK_GENERAL_BODY_X, PICK_GENERAL_BODY_Y, PICK_GENERAL_BODY_W, PICK_GENERAL_BODY_H};

    for (unsigned int i = 0; i < battalion_count * battalion_size; i++) {
        if (!units->is_present[i]) continue;
        consider(result, units->x[i], units->y[i], unit_body, screen_x, screen_y, is_enemy, i / battalion_size);
    }

    for (unsigned int i = 0; i < battalion_count; i++) {
        if (!generals->is_present[i]) continue;
        consider(result, generals->x[i], generals->y[i], general_body, screen_x, screen_y, is_enemy, i);
    }

}

_Bool battlefield_pick_battalion(const Table *player_tables, const Table *enemy_tables, unsigned int battalion_count, unsigned int battalion_size, int screen_x, int screen_y, _Bool *is_enemy, unsigned int *index) {

    PickResult result = {LONG_MAX, false, 0};
    scan_army(&result, player_tables, false, battalion_count, battalion_size, screen_x, screen_y);
    scan_army(&result, enemy_tables, true, battalion_count, battalion_size, screen_x, screen_y);
    if (result.best_distance == LONG_MAX) return false;

    *is_enemy = result.is_enemy;
    *index = result.index;
    return true;

}
