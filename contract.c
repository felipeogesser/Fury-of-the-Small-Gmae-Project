#include "contract.h"
#include "contract_internal.h"
#include "engine_internal.h"
#include "game_state_internal.h"
#include "scenes.h"
#include <stdbool.h>

static const enum Contracts main_menu[] = {

    CONTRACT_UNKNOWN

};

static const enum Contracts battleplan[] = {

    SET_ARMY_POSITION

};

static const enum Contracts battlefield[] = {

    SET_ARMY_POSITION

};

static const enum Contracts *const contracts[SCENE_COUNT] = {

    main_menu,
    battleplan,
    battlefield

};

Contract *contract_set(enum Contracts title, void *body) {

    Contract *contract = calloc(1, sizeof(Contract));
    contract->title = title;
    contract->body = body;
    return contract;

}

_Bool check_contract(Contract *contract) {

    const enum Contracts *contract_list = contracts[engine.game->scene_state.scene];
    _Bool contract_match = false;
    unsigned int i = 0;
    while (contract_list[i] != CONTRACT_UNKNOWN) {

        if (contract_list[i] == contract->title) {

            contract_match = true;
            return contract_match;

        }

        i++;

    }

    return contract_match;

}

void *sign_contract(Contract *contract) {

    return contract->body;

}

void contract_destroy(Contract *contract) {

    free(contract->body);
    free(contract);

}