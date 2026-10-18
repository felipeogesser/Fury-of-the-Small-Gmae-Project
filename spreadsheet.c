#include "spreadsheet.h"
#include "spreadsheet_internal.h"
#include "battalion_types.h"
#include "general_types.h"
#include "sprites_types.h"

/*const BattalionSpreadsheet lancer = {

    .battalion_type = SPEARMEN,
    .sprite_type = LANCER,

    .morale = 100,
    .fatigue = 100,
    .hp = 8000,
    .speed = 40,
    .meele_attack = 30,
    .meele_defense = 40,
    .strength = 35,
    .range = 10,
    .charge_bonus = 10,
    .armor = 30,
    .attack_speed 1,

    .formation_x = 5,
    .formation_y = 10,

    .is_shielded = 0,
    .is_ranged = 0,
    .is_large = 0,
    .is_armor_piercing = 1,
    .is_anti_large = 1

};

const BattalionSpreadsheet warrior = {

    .battalion_type = INFANTRY,
    .sprite_type = WARRIOR,

    .morale = 100,
    .fatigue = 100,
    .hp = 9000,
    .speed = 40,
    .meele_attack = 40,
    .meele_defense = 35,
    .strength = 35,
    .range = 10,
    .charge_bonus = 10,
    .armor = 35,
    .attack_speed 1,

    .formation_x = 5,
    .formation_y = 10,

    .is_shielded = 1,
    .is_ranged = 0,
    .is_large = 0,
    .is_armor_piercing = 0,
    .is_anti_large = 0

};*/

const Spreadsheet spreadsheet = {
    .battalion_spreadsheet = {
        [INFANTRY] = {
            .battalion_type = INFANTRY,
            .sprite_type = WARRIOR,

            .morale = 100,
            .fatigue = 100,
            .hp = 9000,
            .speed = 200,
            .meele_attack = 40,
            .meele_defense = 35,
            .strength = 35,
            .range = 30,
            .charge_bonus = 10,
            .armor = 35,
            .attack_speed = 1,

            .formation_x = 5,
            .formation_y = 10,

            .is_shielded = 1,
            .is_ranged = 0,
            .is_large = 0,
            .is_armor_piercing = 0,
            .is_anti_large = 0
        },
        [SPEARMEN] = {
            .battalion_type = SPEARMEN,
            .sprite_type = LANCER,

            .morale = 100,
            .fatigue = 100,
            .hp = 8000,
            .speed = 200,
            .meele_attack = 30,
            .meele_defense = 40,
            .strength = 35,
            .range = 30,
            .charge_bonus = 10,
            .armor = 30,
            .attack_speed = 1,

            .formation_x = 5,
            .formation_y = 10,

            .is_shielded = 0,
            .is_ranged = 0,
            .is_large = 0,
            .is_armor_piercing = 1,
            .is_anti_large = 1
        }
    }
};

const Spreadsheet *spreadsheet_ptr = &spreadsheet;

BattalionSpreadsheet get_battalion_spreadsheet(enum BattalionType type) {

    return spreadsheet.battalion_spreadsheet[type];

}