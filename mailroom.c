#include "mailroom.h"
#include "mailroom_internal.h"
#include "engine_internal.h"
#include "game_state_internal.h"
#include "letter_types.h"
#include "mailbag.h"
#include "mailbag_internal.h"
#include "mailbox.h"
#include "mailbox_internal.h"
#include "scenes.h"
#include <stddef.h>

Mailroom mailroom = {0};

void mailroom_init(void) {

    for (unsigned int i = 0; i < SCENE_COUNT; i++) {

        mailbox_init(&mailroom.mailbox[i]);

    }

}

Mailbag mailroom_fetch_new_letters(void) {

    Mailbox *mailbox = &mailroom.mailbox[engine.game->scene_state.scene];
    if (mailbox->new_letter_count == 0) {

        Mailbag mailbag = {0};
        return mailbag;

    }

    Letter *letters = mailbox_fetch_new_letters(mailbox);
    Mailbag mailbag = {letters, mailbox->new_letter_count};

    return mailbag;

}

void mailroom_store_letter(Letter *letter, enum Scene scene) {

    Mailbox *mailbox = &mailroom.mailbox[scene];
    mailbox_letter_push(mailbox, letter);


}

void mailroom_save_letter(Letter *letter) {

    Mailbox *mailbox = &mailroom.mailbox[engine.game->scene_state.scene];
    mailbox_letter_push(mailbox, letter);
    mailbox->new_letter_count = 0;

}