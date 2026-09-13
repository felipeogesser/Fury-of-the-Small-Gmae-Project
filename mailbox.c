#include "mailbox.h"
#include "mailbox_internal.h"
#include "letter.h"
#include "letter_internal.h"
#include <stddef.h>
#include <stdlib.h>

// private prototypes
static void mailbox_letter_stack_push_front(Mailbox *mailbox);
static void mailbox_letter_stack_pop_front(Mailbox *mailbox);

void mailbox_init(Mailbox *mailbox) {

    mailbox->first_stack.previous_stack = NULL;
    mailbox->first_stack.next_stack = NULL;
    mailbox->last_stack = &mailbox->first_stack;
    mailbox->first_new_letter.letter_stack = NULL;
    mailbox->stack_count = 1;

}

void mailbox_letter_push(Mailbox *mailbox, Letter *letter) {

    if (mailbox->letter_count / mailbox->stack_count == LETTER_STACK_SIZE) {

        mailbox_letter_stack_push_front(mailbox);

    }

    unsigned int idx = mailbox->letter_count - (mailbox->stack_count - 1) * LETTER_STACK_SIZE;
    mailbox->last_stack->letter[idx] = *letter;
    mailbox->letter_count++;
    if (mailbox->new_letter_count == 0) {

        mailbox->first_new_letter.letter_stack = mailbox->last_stack;
        mailbox->first_new_letter.letter_idx = idx;

    }
    mailbox->new_letter_count++;

}

void mailbox_letter_pop(Mailbox *mailbox) {

    mailbox->letter_count--;
    unsigned int idx = mailbox->letter_count - (mailbox->stack_count - 1) * LETTER_STACK_SIZE;
    Letter *letter = &mailbox->last_stack->letter[idx];
    letter_destroy(letter);

    if ((mailbox->letter_count - 1) / mailbox->stack_count == LETTER_STACK_SIZE) {

        mailbox_letter_stack_pop_front(mailbox);

    }

}

Letter *mailbox_fetch_new_letters(Mailbox *mailbox) {

    Letter *letters = calloc(mailbox->new_letter_count, sizeof(Letter));

    unsigned int letter_idx = mailbox->first_new_letter.letter_idx;
    LetterStackList *current_stack = mailbox->first_new_letter.letter_stack;
    unsigned int new_letter_count = mailbox->new_letter_count;
    for (unsigned int i = 0; new_letter_count > 0; new_letter_count--) {

        if (letter_idx == LETTER_STACK_SIZE) {

            letter_idx = 0;
            current_stack = current_stack->next_stack;

        }

        letters[i++] = current_stack->letter[letter_idx++];

    }

    for (unsigned int i = 0; i < mailbox->new_letter_count; i++) {

        mailbox_letter_pop(mailbox);

    }

    return letters;

}

static void mailbox_letter_stack_push_front(Mailbox *mailbox) {

    mailbox->last_stack->next_stack = calloc(1, sizeof(LetterStackList));
    mailbox->last_stack->next_stack->previous_stack = mailbox->last_stack;
    mailbox->last_stack = mailbox->last_stack->next_stack;
    mailbox->stack_count++;

}

static void mailbox_letter_stack_pop_front(Mailbox *mailbox) {

    mailbox->last_stack = mailbox->last_stack->previous_stack;
    free(mailbox->last_stack->next_stack);
    mailbox->last_stack->next_stack = NULL;
    mailbox->stack_count--;

}

