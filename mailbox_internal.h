#ifndef MAILBOX_INTERNAL_H
#define MAILBOX_INTERNAL_H

#include "mailbox_types.h"
#include "letter_internal.h"

#define LETTER_STACK_SIZE 5

typedef struct LetterBookmark {

    LetterStackList *letter_stack;
    unsigned int letter_idx;

} LetterBookmark;

typedef struct LetterStackList {

    Letter letter[LETTER_STACK_SIZE];
    LetterStackList *previous_stack;
    LetterStackList *next_stack;

} LetterStackList;

typedef struct Mailbox {

    LetterStackList first_stack;
    LetterStackList *last_stack;
    LetterBookmark first_new_letter;
    unsigned int stack_count;
    unsigned int letter_count;
    unsigned int new_letter_count;

} Mailbox;

#endif