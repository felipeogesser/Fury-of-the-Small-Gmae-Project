#ifndef LETTER_H
#define LETTER_H

#include "letter_types.h"
#include "contract_types.h"
#include "scenes.h"

Letter *letter_write(enum Scene source_scene, enum Scene destination_scene, Contract *contract);
void letter_send(Letter *letter);
void letter_destroy(Letter *letter);

#endif