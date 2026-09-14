#include "letter.h"
#include "letter_internal.h"
#include "contract.h"
#include "contract_types.h"
#include "game_state_internal.h"
#include "mailroom.h"
#include "scenes.h" 
#include <stdlib.h>

Letter *letter_write(enum Scene source_scene, enum Scene destination_scene, Contract *contract) {

    Letter *letter = calloc(1, sizeof(Letter));
    letter->contract = contract;
    //contract_destroy(contract);
    letter->from = source_scene;
    letter->to = destination_scene;
    return letter;

}

void letter_send(Letter *letter) {

    mailroom_store_letter(letter);

}

void letter_destroy(Letter *letter) {

    contract_destroy(letter->contract);
    free(letter);

}

/*void letter_write(Letter *letter, Contract *contract, void *obj, unsigned int count, enum Scene destination_scene, FieldEntry *field_entry) {

    letter.contract = *contract;
    letter.contract =
    letter.contract->body = 
    letter->from = engine.game->scene_state.scene;
    letter->to = destination_scene;
    unsigned int field_table_count = field_table_fields_count(field_entry);
    size_t obj_size = field_table_obj_type_size(field_entry);
    letter->body = calloc(count, obj_size);
    memcpy(letter->body, obj, count * obj_size);
    letter->field_entry = calloc(field_table_count + 1, sizeof(FieldEntry)); // 1 adds space for obj_size field, which is not accounted for in field_table_fields_count
    memcpy(letter->field_entry, field_entry, count * (field_table_count + 1));

}*/