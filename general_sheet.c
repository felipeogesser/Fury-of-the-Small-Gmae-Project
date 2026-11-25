#include "general_sheet.h"
#include "general_internal.h"
#include "spreadsheet.h"
#include "ui.h"

const char *general_sheet_type_name(enum BattalionType type) {

    switch (type) {
        case TEMPLAR: return "Templar";
        case CAVALRY: return "Cavalry";
        case GUARDSMEN: return "Guardsmen";
        case SWORDSMEN: return "Swordsmen";
        case ARCHERS: return "Archers";
        default: return "Battalion";
    }

}

const char *general_sheet_rarity_name(enum Rarity rarity) {

    switch (rarity) {
        case UNIQUE: return "Unique";
        case RARE: return "Rare";
        case COMMON: return "Common";
        default: return "";
    }

}

void general_sheet_fill(StatSheet *sheet, const General *general) {

    const BattalionSpreadsheet battalion = get_battalion_spreadsheet(general->battalion_type);

    sheet->title = general_sheet_type_name(general->battalion_type);

    stat_sheet_row(sheet, "Health per man", "%.0f", (double)battalion.hp);
    stat_sheet_row(sheet, "Morale", "%.0f", (double)battalion.morale);
    stat_sheet_row(sheet, "Stamina", "%.0f", (double)battalion.fatigue);
    stat_sheet_row(sheet, "Speed", "%.0f", (double)battalion.speed);
    stat_sheet_row(sheet, "Melee attack", "%.0f", (double)battalion.meele_attack);
    stat_sheet_row(sheet, "Melee defense", "%.0f", (double)battalion.meele_defense);
    stat_sheet_row(sheet, "Strength", "%.0f", (double)battalion.strength);
    stat_sheet_row(sheet, "Armor", "%.0f", (double)battalion.armor);
    stat_sheet_row(sheet, "Range", "%.0f", (double)battalion.range);
    stat_sheet_row(sheet, "Charge bonus", "%.0f", (double)battalion.charge_bonus);
    stat_sheet_row(sheet, "Attack speed", "%.2f", (double)battalion.attack_speed);
    stat_sheet_row(sheet, "General health", "%d", general->hp);
    stat_sheet_row(sheet, "General attack", "%d", general->base_meele_attack);

    if (battalion.is_shielded) stat_sheet_chip(sheet, "Shielded", ui_color_text);
    if (battalion.is_ranged) stat_sheet_chip(sheet, "Ranged", ui_color_text);
    if (battalion.is_large) stat_sheet_chip(sheet, "Large", ui_color_text);
    if (battalion.is_armor_piercing) stat_sheet_chip(sheet, "Armor piercing", ui_color_text);
    if (battalion.is_anti_large) stat_sheet_chip(sheet, "Anti large", ui_color_text);

}
