#ifndef LETTER_H
#define LETTER_H

#include "letter_types.h"
#include "field_entry.h"
#include "scenes.h"

void letter_write(Letter *letter, void *obj, unsigned int count, enum Scene destination_scene, FieldEntry *field_entry);
void letter_send(Letter *letter);
void letter_destroy(Letter *letter);

#endif