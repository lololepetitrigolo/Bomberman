#ifndef DEBUG_DRAW_H
#define DEBUG_DRAW_H

#include <SDL.h>
#include <SDL_ttf.h>

void debug_init(void);
void debug_quit(void);

/* Print formatted text on the window for one frame. */
void screen_printf(const char* format, ...);

void debug_draw(SDL_Renderer* renderer);

#endif
