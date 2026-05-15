#include "init.h"

void memory_arena_init(void);
void engine_init(void);
void main_menu_init(void);

void init(void) {
    
    memory_arena_init();
    engine_init();
    main_menu_init();

}
