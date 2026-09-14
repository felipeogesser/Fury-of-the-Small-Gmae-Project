#ifndef MAILBOX_H
#define MAILBOX_H

#include "mailbox_types.h"
#include "letter_types.h"

void mailbox_init(Mailbox *mailbox);
void mailbox_letter_push(Mailbox *mailbox, Letter *letter);
void mailbox_letter_pop(Mailbox *mailbox);
Letter *mailbox_fetch_new_letters(Mailbox *mailbox);

#endif