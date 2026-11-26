#ifndef GENERAL_SHEET_H
#define GENERAL_SHEET_H

#include "battalion_types.h"
#include "general_types.h"
#include "stat_sheet.h"

#define GENERAL_SHEET_ROWS 13u

const char *general_sheet_type_name(enum BattalionType type);
const char *general_sheet_rarity_name(enum Rarity rarity);
void general_sheet_fill(StatSheet *sheet, const General *general);

#endif
