#include "general_card.h"
#include "animation_types.h"
#include "engine_internal.h"
#include "general_sheet.h"
#include "spreadsheet_internal.h"
#include "sprites_internal.h"
#include "ui.h"

#define CARD_FRAME_MS 140
#define CARD_NAME_GAP 8

static const SDL_Rect sprite_crop = {27, 22, 46, 46};

static _Bool is_known(enum GeneralType type) {

    return type > GENERAL_PADDING_FOR_ZERO && type < GENERAL_UNIQUE;

}

const char *general_card_name(enum GeneralType type) {

    if (!is_known(type)) return "Unknown";
    return general_sheet_type_name(spreadsheet_ptr->general_spreadsheet[type].battalion_type);

}

void general_card_render_sprite(enum GeneralType type, SDL_Rect box) {

    if (!is_known(type) || engine.sprite_pack == NULL) return;

    const enum Sprites sprite_type = spreadsheet_ptr->general_spreadsheet[type].sprite_type;
    const Sprite *sprite = &engine.sprite_pack->sprite[sprite_type][IDLE];
    if (sprite->texture == NULL || sprite->frames_count == 0) return;

    const int frame_w = sprite->width / sprite->frames_count;
    const unsigned int frame = (SDL_GetTicks() / CARD_FRAME_MS) % sprite->frames_count;
    const SDL_Rect crop = {(int)frame * frame_w + sprite_crop.x, sprite_crop.y, sprite_crop.w, sprite_crop.h};
    SDL_RenderCopy(engine.renderer, sprite->texture, &crop, &box);

}

void general_card_render(enum GeneralType type, SDL_Rect rect, _Bool is_selected, _Bool is_hovered) {

    SDL_Color fill = {36, 38, 46, 255};
    if (is_hovered) fill = (SDL_Color){52, 50, 44, 255};
    if (is_selected) fill = (SDL_Color){70, 58, 34, 255};

    ui_fill((SDL_Rect){rect.x + 4, rect.y + 6, rect.w, rect.h}, (SDL_Color){0, 0, 0, 90});
    ui_fill(rect, fill);

    const int name_h = ui_font_height(UI_FONT_SMALL);
    const int art = rect.w - 16 < rect.h - name_h - 2 * CARD_NAME_GAP - 8 ? rect.w - 16 : rect.h - name_h - 2 * CARD_NAME_GAP - 8;
    const SDL_Rect art_box = {rect.x + (rect.w - art) / 2, rect.y + 8, art, art};
    ui_fill(art_box, (SDL_Color){22, 23, 28, 255});
    general_card_render_sprite(type, art_box);

    ui_text(UI_FONT_SMALL, general_card_name(type), rect.x + rect.w / 2, art_box.y + art_box.h + CARD_NAME_GAP, ui_color_text, UI_ALIGN_CENTER);

    SDL_Color edge = ui_color_panel_edge;
    if (is_hovered) edge = (SDL_Color){ui_color_accent.r, ui_color_accent.g, ui_color_accent.b, 160};
    if (is_selected) edge = ui_color_accent;
    ui_frame(rect, edge, is_selected ? 3 : 1);

}
