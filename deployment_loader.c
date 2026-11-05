#include "deployment_loader.h"
#include "battleplan_grid_types.h"
#include "battleplan_internal.h"
#include "file_io.h"
#include "general_internal.h"
#include "spreadsheet_internal.h"
#include <math.h>

// private prototypes
static void load_deployment_layout_with_csv_data(unsigned int *deployment_layout, const char *csv_buffer);
static void load_generals_from_player_deployment_layout(Battleplan *battleplan, unsigned int (*deployment_layout)[GRID_DIMENSION_Y]);
static void load_generals_from_enemy_deployment_layout(Battleplan *battleplan, unsigned int (*deployment_layout)[GRID_DIMENSION_Y]);
static General convert_spreadsheet_general_to_general(enum GeneralType general_type);
static void flip_deployment_on_x_axis(unsigned int (*deployment_layout)[GRID_DIMENSION_Y]);

void load_generals_from_csv_deployment_layout_to_player_board(Battleplan *battleplan, const char *file_name) {

    unsigned int deployment_layout[GRID_DIMENSION_X][GRID_DIMENSION_Y] = {0};

    char *csv_buffer = open_read_close_file(file_name);

    load_deployment_layout_with_csv_data((unsigned int *)deployment_layout, csv_buffer);

    free(csv_buffer);

    load_generals_from_player_deployment_layout(battleplan, deployment_layout);


}

void load_generals_from_csv_deployment_layout_to_enemy_board(Battleplan *battleplan, const char *file_name) {

    unsigned int deployment_layout[GRID_DIMENSION_X][GRID_DIMENSION_Y] = {0};

    char *csv_buffer = open_read_close_file(file_name);

    load_deployment_layout_with_csv_data((unsigned int *)deployment_layout, csv_buffer);

    free(csv_buffer);

    flip_deployment_on_x_axis(deployment_layout);

    load_generals_from_enemy_deployment_layout(battleplan, deployment_layout);


}

static void load_deployment_layout_with_csv_data(unsigned int *deployment_layout, const char *csv_buffer) {

    unsigned int deployment_idx = 0;
    const char *p = csv_buffer;
    while (*p != '\0' && deployment_idx < GRID_DIMENSION_X * GRID_DIMENSION_Y) {

        if (*p != ',' && *p != '\n') {

            const char *copy_p = p;
            unsigned int number_lengh = 0;
            while (*copy_p != ',' && *copy_p != '\n' && *copy_p != '\0') {
                number_lengh++;
                copy_p++;
            }

            unsigned int number = 0;
            for (unsigned int i = 0; i < number_lengh; i++) {

                char c = *p;
                unsigned int digit = c - '0';
                number += digit * pow(10, (number_lengh -1) - i);
                p++;

            }
            deployment_layout[(deployment_idx % GRID_DIMENSION_X) * GRID_DIMENSION_Y + deployment_idx / GRID_DIMENSION_X] = number;
            deployment_idx++;

        }
        
        p += (*p != '\0');

    }

}

static void load_generals_from_player_deployment_layout(Battleplan *battleplan, unsigned int (*deployment_layout)[GRID_DIMENSION_Y]) {

    unsigned int general_count = 0;
    for (unsigned int i = 0; i < GRID_DIMENSION_X; i++) {
        for (unsigned int j = 0; j < GRID_DIMENSION_Y; j++) {
            general_count += (deployment_layout[i][j] != 0);
        }
    }
    battleplan->player_board.general_count = general_count;
    
    battleplan->player_inventory_general = calloc(general_count, sizeof(General));

    unsigned int general_idx = 0;
    for (unsigned int i = 0; i < GRID_DIMENSION_X; i++) {
        for (unsigned int j = 0; j < GRID_DIMENSION_Y; j++) {
            if (deployment_layout[i][j] != 0) {
                enum GeneralType general_type = deployment_layout[i][j];
                battleplan->player_inventory_general[general_idx] = convert_spreadsheet_general_to_general(general_type);
                battleplan->player_board.grid[i][j] = &battleplan->player_inventory_general[general_idx];
                general_idx++;
            }
        }
    }

}

static void load_generals_from_enemy_deployment_layout(Battleplan *battleplan, unsigned int (*deployment_layout)[GRID_DIMENSION_Y]) {

    unsigned int general_count = 0;
    for (unsigned int i = 0; i < GRID_DIMENSION_X; i++) {
        for (unsigned int j = 0; j < GRID_DIMENSION_Y; j++) {
            general_count += (deployment_layout[i][j] != 0);
        }
    }
    battleplan->enemy_board.general_count = general_count;
    
    battleplan->stage_enemy_general = calloc(general_count, sizeof(General));

    unsigned int general_idx = 0;
    for (unsigned int i = 0; i < GRID_DIMENSION_X; i++) {
        for (unsigned int j = 0; j < GRID_DIMENSION_Y; j++) {
            if (deployment_layout[i][j] != 0) {
                enum GeneralType general_type = deployment_layout[i][j];
                battleplan->stage_enemy_general[general_idx] = convert_spreadsheet_general_to_general(general_type);
                battleplan->enemy_board.grid[i][j] = &battleplan->stage_enemy_general[general_idx];
                general_idx++;
            }
        }
    }

}

static General convert_spreadsheet_general_to_general(enum GeneralType general_type) {

    return (General){
        .sprite.type = spreadsheet_ptr->general_spreadsheet[general_type].sprite_type,
        .sprite_type = spreadsheet_ptr->general_spreadsheet[general_type].sprite_type,
        .hp = spreadsheet_ptr->general_spreadsheet[general_type].hp,
        .base_meele_attack = spreadsheet_ptr->general_spreadsheet[general_type].meele_attack,
        .base_meele_defense = spreadsheet_ptr->general_spreadsheet[general_type].meele_defense,
        .armor = spreadsheet_ptr->general_spreadsheet[general_type].armor,
        .base_strength = spreadsheet_ptr->general_spreadsheet[general_type].strength,
        .attack_speed = spreadsheet_ptr->general_spreadsheet[general_type].attack_speed,
        .general_type = spreadsheet_ptr->general_spreadsheet[general_type].general_type,
        .battalion_type = spreadsheet_ptr->general_spreadsheet[general_type].battalion_type,
        .units_type = spreadsheet_ptr->general_spreadsheet[general_type].units_type,
        .range = spreadsheet_ptr->general_spreadsheet[general_type].range,
        .charge_bonus = spreadsheet_ptr->general_spreadsheet[general_type].charge_bonus,
        .is_shielded = spreadsheet_ptr->general_spreadsheet[general_type].is_shielded,
        .is_ranged = spreadsheet_ptr->general_spreadsheet[general_type].is_ranged,
        .is_large = spreadsheet_ptr->general_spreadsheet[general_type].is_large,
        .is_armor_piercing = spreadsheet_ptr->general_spreadsheet[general_type].is_armor_piercing,
        .is_anti_large = spreadsheet_ptr->general_spreadsheet[general_type].is_anti_large
    };

}

static void flip_deployment_on_x_axis(unsigned int (*deployment_layout)[GRID_DIMENSION_Y]) {

    for (unsigned int x = 0; x < GRID_DIMENSION_X / 2; x++) {
        for (unsigned int y = 0; y < GRID_DIMENSION_Y; y++) {

            unsigned int tmp = deployment_layout[x][y];
            deployment_layout[x][y] = deployment_layout[GRID_DIMENSION_X - 1 - x][y];
            deployment_layout[GRID_DIMENSION_X - 1 - x][y] = tmp;

        }
    }

}
