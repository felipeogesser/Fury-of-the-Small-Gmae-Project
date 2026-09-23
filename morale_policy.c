#include "morale_policy.h"
#include "morale_types.h"

// private prototypes
static signed int army_losses(signed int balance_of_power);
static signed int damage_sustained(unsigned int max_hp, unsigned int current_hp);


enum MoraleState evaluate_morale_state(unsigned int base, signed int current) {

    unsigned int aux = base / MORALE_STATE_COUNT;
    signed int result = ((signed int)base - current + (aux)) / aux - 1;
    enum MoraleState state = result * !(result < 0);
    state = (state < MORALE_STATE_COUNT) ? state : (MORALE_STATE_COUNT - 1);

    return state;

}

signed int evaluate_morale_static_modifiers(unsigned int modifiers) {

    signed int delta = 0;

    delta += (modifiers & FLAG_SORROUNDED)               ?   VALUE_SORROUNDED                : 0;
    delta += (modifiers & FLAG_GENERAL_WOUNDED)          ?   VALUE_GENERAL_WOUNDED           : 0;
    delta += (modifiers & FLAG_FLANKED)                  ?   VALUE_FLANKED                   : 0;
    delta += (modifiers & FLAG_STRONGER_ENEMIES_NEARBY)  ?   VALUE_STRONGER_ENEMIES_NEARBY   : 0;
    delta += (modifiers & FLAG_LOSING_CURRENT_COMBAT)    ?   VALUE_LOSING_CURRENT_COMBAT     : 0;
    delta += (modifiers & FLAG_BEING_CHARGED_AT)         ?   VALUE_BEING_CHARGED_AT          : 0;
  //delta += (modifiers & FLAG_ARMY_LOSSES)              ?   VALUE_ARMY_LOSSES               : 0;
  //delta += (modifiers & FLAG_DAMAGE_SUSTAINED)         ?   VALUE_DAMAGE_SUSTAINED          : 0;
    delta += (modifiers & FLAG_FLANKS_SECURED)           ?   VALUE_FLANKS_SECURED            : 0;
    delta += (modifiers & FLAG_WINNING_CURRENT_COMBAT)   ?   VALUE_WINNING_CURRENT_COMBAT    : 0;

    return delta;

}

signed int evaluate_morale_dynamic_modifiers(unsigned int modifiers, unsigned int max_hp, unsigned int current_hp, signed int balance_of_power) {

    signed int delta = 0;
    if (modifiers & FLAG_ARMY_LOSSES) {

        delta += army_losses(balance_of_power);

    }
    if (modifiers & FLAG_DAMAGE_SUSTAINED) {

        delta += damage_sustained(max_hp, current_hp);
    
    }
    return delta;

}

static signed int army_losses(signed int balance_of_power) {

    return balance_of_power * VALUE_ARMY_LOSSES;

}

static signed int damage_sustained(unsigned int max_hp, unsigned int current_hp) {

    return (max_hp / (current_hp + 1)) * VALUE_DAMAGE_SUSTAINED;

}
