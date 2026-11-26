#ifndef RUN_SUMMARY_H
#define RUN_SUMMARY_H

#include <SDL2/SDL.h>

void run_summary_reset(void);
_Bool run_summary_is_open(void);
_Bool run_summary_input(const SDL_Event *e);
void run_summary_render(void);

#endif
