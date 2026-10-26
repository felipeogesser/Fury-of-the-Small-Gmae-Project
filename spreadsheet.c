#include "spreadsheet.h"
#include "spreadsheet_internal.h"
#include "battalion_types.h"
#include "general_types.h"
#include "sprites_types.h"

const Spreadsheet spreadsheet = {
    .battalion_spreadsheet = {
        [TEMPLAR] = {
            .battalion_type = TEMPLAR,
            .sprite_type = SPRITE_TEMPLAR,

            .morale = 110,
            .fatigue = 90,
            .hp = 10000,
            .speed = 170,
            .meele_attack = 42,
            .meele_defense = 38,
            .strength = 38,
            .range = 30,
            .charge_bonus = 12,
            .armor = 75,
            .attack_speed = 1.2f,

            .formation_x = 5,
            .formation_y = 10,

            .is_shielded = 0,
            .is_ranged = 0,
            .is_large = 0,
            .is_armor_piercing = 0,
            .is_anti_large = 0
        },
        [CAVALRY] = {
            .battalion_type = CAVALRY,
            .sprite_type = SPRITE_CAVALRY,

            .morale = 100,
            .fatigue = 100,
            .hp = 8500,
            .speed = 380,
            .meele_attack = 34,
            .meele_defense = 22,
            .strength = 30,
            .range = 35,
            .charge_bonus = 55,
            .armor = 45,
            .attack_speed = 1,

            .formation_x = 2,
            .formation_y = 25,

            .is_shielded = 0,
            .is_ranged = 0,
            .is_large = 1,
            .is_armor_piercing = 0,
            .is_anti_large = 0
        },
        [GUARDSMEN] = {
            .battalion_type = GUARDSMEN,
            .sprite_type = SPRITE_GUARDSMEN,

            .morale = 100,
            .fatigue = 100,
            .hp = 9000,
            .speed = 190,
            .meele_attack = 28,
            .meele_defense = 42,
            .strength = 28,
            .range = 35,
            .charge_bonus = 8,
            .armor = 40,
            .attack_speed = 1,

            .formation_x = 5,
            .formation_y = 10,

            .is_shielded = 1,
            .is_ranged = 0,
            .is_large = 0,
            .is_armor_piercing = 0,
            .is_anti_large = 1
        },
        [SWORDSMEN] = {
            .battalion_type = SWORDSMEN,
            .sprite_type = SPRITE_SWORDSMEN,

            .morale = 95,
            .fatigue = 100,
            .hp = 8000,
            .speed = 210,
            .meele_attack = 38,
            .meele_defense = 28,
            .strength = 30,
            .range = 30,
            .charge_bonus = 12,
            .armor = 25,
            .attack_speed = 1,

            .formation_x = 5,
            .formation_y = 10,

            .is_shielded = 0,
            .is_ranged = 0,
            .is_large = 0,
            .is_armor_piercing = 1,
            .is_anti_large = 0
        }
    },

    .general_spreadsheet = {
        [GENERAL_GUARDSMEN] = {

                .general_type = GENERAL_GUARDSMEN,
                .sprite_type = SPRITE_GUARDSMEN,
                .battalion_type = GUARDSMEN,
                .units_type = SPRITE_GUARDSMEN,
                .hp = 1000,
                .meele_attack = 15,
                .meele_defense = 14,
                .strength = 10,
                .range = 22,
                .charge_bonus = 8,
                .armor = 8,
                .attack_speed = 1,

                .is_shielded = 1,
                .is_ranged = 0,
                .is_large = 0,
                .is_armor_piercing = 0,
                .is_anti_large = 1

        },
        [GENERAL_SWORDSMEN] = {

                .general_type = GENERAL_SWORDSMEN,
                .sprite_type = SPRITE_SWORDSMEN,
                .battalion_type = SWORDSMEN,
                .units_type = SPRITE_SWORDSMEN,
                .hp = 900,
                .meele_attack = 20,
                .meele_defense = 10,
                .strength = 12,
                .range = 20,
                .charge_bonus = 10,
                .armor = 5,
                .attack_speed = 1,

                .is_shielded = 0,
                .is_ranged = 0,
                .is_large = 0,
                .is_armor_piercing = 1,
                .is_anti_large = 0

        },
        [GENERAL_TEMPLAR] = {

                .general_type = GENERAL_TEMPLAR,
                .sprite_type = SPRITE_TEMPLAR,
                .battalion_type = TEMPLAR,
                .units_type = SPRITE_TEMPLAR,
                .hp = 1200,
                .meele_attack = 20,
                .meele_defense = 15,
                .strength = 14,
                .range = 20,
                .charge_bonus = 10,
                .armor = 15,
                .attack_speed = 1.2f,

                .is_shielded = 0,
                .is_ranged = 0,
                .is_large = 0,
                .is_armor_piercing = 0,
                .is_anti_large = 0

        },
        [GENERAL_CAVALRY] = {

                .general_type = GENERAL_CAVALRY,
                .sprite_type = SPRITE_CAVALRY,
                .battalion_type = CAVALRY,
                .units_type = SPRITE_CAVALRY,
                .hp = 1000,
                .meele_attack = 18,
                .meele_defense = 8,
                .strength = 12,
                .range = 24,
                .charge_bonus = 30,
                .armor = 10,
                .attack_speed = 1,

                .is_shielded = 0,
                .is_ranged = 0,
                .is_large = 1,
                .is_armor_piercing = 0,
                .is_anti_large = 0

        }
    }
};

const Spreadsheet *spreadsheet_ptr = &spreadsheet;

BattalionSpreadsheet get_battalion_spreadsheet(enum BattalionType type) {

    return spreadsheet.battalion_spreadsheet[type];

}