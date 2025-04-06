#ifndef BUTTON_H
#define BUTTON_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>

typedef void (*ButtonCallbackFn)(void*, void*);

typedef struct ButtonCallback {
	ButtonCallbackFn fn;
	void* data1;
	void* data2;
} ButtonCallback;

typedef struct Button {
	SDL_Rect rect;
	ButtonCallback on_click;
	char* text;
	TTF_Font* font;
	bool hovered;
} Button;

void button_init(Button* button,
				 SDL_Rect rect,
				 ButtonCallbackFn on_click,
				 void* on_click_data,
				 char* text,
				 TTF_Font* font);

void button_draw(Button* button, SDL_Renderer* renderer);

void button_update(Button* button);

#endif
