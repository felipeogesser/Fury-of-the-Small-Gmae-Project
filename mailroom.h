#ifndef MAILROOM_H
#define MAILROOM_H

#include "letter_types.h"
#include "mailbag_types.h"
#include "scenes.h"

void mailroom_init(void);
Mailbag mailroom_fetch_new_letters(void);
void mailroom_store_letter(Letter *letter);
void mailroom_save_letter(Letter *letter);

#endif