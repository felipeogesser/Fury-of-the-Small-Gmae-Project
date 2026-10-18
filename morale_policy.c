#include "morale_policy.h"
#include "morale_types.h"

// private prototypes
static signed int army_losses(signed int balance_of_power);
static signed int damage_sustained(unsigned int max_hp, unsigned int current_hp);
static _Bool is_army_losing(signed int balance_of_power);
static _Bool is_damage_sustained(unsigned int max_hp, unsigned int current_hp);

enum MoraleState evaluate_morale_state(unsigned int base, signed int current) {

    unsigned int bucket_size = base / MORALE_STATE_COUNT;
    signed int result = ((signed int)base - current + (bucket_size)) / bucket_size - 1;
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

    if (max_hp == 0) return 0;
    if (current_hp > max_hp) current_hp = max_hp;
    signed int lost_percent = (signed int)((max_hp - current_hp) * 100 / max_hp);
    return lost_percent * VALUE_DAMAGE_SUSTAINED;

}

unsigned int evaluate_morale_dynamic_flags(unsigned int modifiers, unsigned int max_hp, unsigned int current_hp, signed int balance_of_power) {

    modifiers &= ~(unsigned int)(FLAG_ARMY_LOSSES | FLAG_DAMAGE_SUSTAINED);
    if (is_army_losing(balance_of_power)) modifiers |= FLAG_ARMY_LOSSES;
    if (is_damage_sustained(max_hp, current_hp)) modifiers |= FLAG_DAMAGE_SUSTAINED;
    return modifiers;

}

static _Bool is_army_losing(signed int balance_of_power) {

    #define ARMY_LOSSES_THRESHOLD 2
    _Bool is_losing = balance_of_power >= ARMY_LOSSES_THRESHOLD;
    #undef ARMY_LOSSES_THRESHOLD
    return is_losing;

}

static _Bool is_damage_sustained(unsigned int max_hp, unsigned int current_hp) {

    #define DAMAGE_SUSTAINED_THRESHOLD 10
    if (max_hp == 0) return 0;
    if (current_hp > max_hp) current_hp = max_hp;
    unsigned int lost_percent = (max_hp - current_hp) * 100 / max_hp;
    _Bool is_damaged = lost_percent >= DAMAGE_SUSTAINED_THRESHOLD;
    #undef DAMAGE_SUSTAINED_THRESHOLD
    return is_damaged;

}