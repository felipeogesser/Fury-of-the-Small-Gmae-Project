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
    letter->from = source_scene;
    letter->to = destination_scene;
    return letter;

}

void letter_send(Letter *letter) {

    mailroom_store_letter(letter);

}

void letter_destroy(Letter *letter) {

    contract_destroy(letter->contract);
    Letter letter_aux = {0};
    *letter = letter_aux;
    //free(letter);

}
