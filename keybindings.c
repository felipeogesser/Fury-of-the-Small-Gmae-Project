#include "keybindings.h"
#include "ui.h"

#define KEYBINDINGS_TITLE_H 40
#define KEYBINDINGS_ROW_H 30
#define KEYBINDINGS_KEY_W 200
#define KEYBINDINGS_COLUMN_GAP 60

static const Keybinding general_bindings[] = {
    {"H / F1", "Show or hide the controls"},
    {"Esc", "Close a stats panel or go back"}
};

static const Keybinding deployment_bindings[] = {
    {"Hold and drag", "Move a general"},
    {"Click a general", "Show the general's stats"},
    {"Click the drawer", "Open or close the reserves"}
};

static const Keybinding battle_bindings[] = {
    {"Space", "Pause or resume the battle"},
    {"1 / 2 / 3", "Battle speed 1x / 2x / 4x"},
    {"Arrow keys", "Move the camera"},
    {"Shift", "Move the camera faster"},
    {"Mouse wheel", "Zoom in and out"},
    {"Hold A", "Mark every unit by army"},
    {"Tab", "Switch left and right army cards"},
    {"Click a card", "Show the battalion's stats"},
    {"Click a unit", "Select the unit's battalion"}
};

static const KeybindingGroup groups[] = {
    {"Everywhere", general_bindings, sizeof(general_bindings) / sizeof(general_bindings[0])},
    {"Deployment", deployment_bindings, sizeof(deployment_bindings) / sizeof(deployment_bindings[0])},
    {"Battle", battle_bindings, sizeof(battle_bindings) / sizeof(battle_bindings[0])}
};

#define GROUP_COUNT (sizeof(groups) / sizeof(groups[0]))

unsigned int keybinding_group_count(void) {

    return GROUP_COUNT;

}

const KeybindingGroup *keybinding_group(unsigned int index) {

    return index < GROUP_COUNT ? &groups[index] : NULL;

}

static int group_height(const KeybindingGroup *group) {

    return KEYBINDINGS_TITLE_H + (int)group->count * KEYBINDINGS_ROW_H;

}

int keybindings_height(void) {

    const int left = group_height(&groups[0]) + 24 + group_height(&groups[1]);
    const int right = group_height(&groups[2]);
    return left > right ? left : right;

}

static void render_group(const KeybindingGroup *group, int x, int y, int w) {

    ui_text(UI_FONT_BUTTON, group->title, x, y, ui_color_accent, UI_ALIGN_LEFT);
    ui_fill((SDL_Rect){x, y + KEYBINDINGS_TITLE_H - 8, w, 1}, ui_color_panel_edge);
    y += KEYBINDINGS_TITLE_H;

    for (unsigned int i = 0; i < group->count; i++) {

        const int row_y = y + (int)i * KEYBINDINGS_ROW_H;
        if (i % 2 == 0) ui_fill((SDL_Rect){x - 6, row_y - 2, w + 12, KEYBINDINGS_ROW_H}, (SDL_Color){255, 255, 255, 8});
        ui_text(UI_FONT_SMALL, group->bindings[i].key, x, row_y + 2, ui_color_text, UI_ALIGN_LEFT);
        ui_text(UI_FONT_SMALL, group->bindings[i].action, x + KEYBINDINGS_KEY_W, row_y + 2, ui_color_muted, UI_ALIGN_LEFT);

    }

}

void keybindings_render(SDL_Rect area) {

    const int column_w = (area.w - KEYBINDINGS_COLUMN_GAP) / 2;
    render_group(&groups[0], area.x, area.y, column_w);
    render_group(&groups[1], area.x, area.y + group_height(&groups[0]) + 24, column_w);
    render_group(&groups[2], area.x + column_w + KEYBINDINGS_COLUMN_GAP, area.y, column_w);

}
