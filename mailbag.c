#include "mailbag.h"
#include "mailbag_internal.h"
#include "letter.h"
#include "letter_internal.h"
#include <stddef.h>
#include <stdlib.h>

void mailbag_letters_destroy(Mailbag *mailbag) {

    for (unsigned int i = 0; i < mailbag->new_letter_count; i++) {

        letter_destroy(&mailbag->letters[i]);

    }
    free(mailbag->letters);
    mailbag->letters = NULL;
    //letter_destroy(&mailbag->letters);

}