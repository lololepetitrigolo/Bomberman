#ifndef TEXT_INPUT_H
#define TEXT_INPUT_H

typedef struct TextInput TextInput;

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>

#define TEXT_INPUT_CAPACITY 32

typedef void (*TextInputCallbackFn)(void*, void*);

typedef struct TextInputCallback {
	TextInputCallbackFn fn;
	void* data1;
	void* data2;
} TextInputCallback;


struct TextInput {
	char text[TEXT_INPUT_CAPACITY];
	uint text_len;
	SDL_Rect rect;
	SDL_Color background_color;
	SDL_Color text_color;
	TTF_Font* font;
	int font_size;
	TextInputCallback on_return;
	bool hovered;
};

void text_input_init(TextInput* input,
					 char* text,
					 SDL_Rect position,
					 SDL_Color background_color,
					 SDL_Color text_color,
					 TTF_Font* font,
					 int font_size);

void text_input_draw(TextInput* input, SDL_Renderer* renderer);

void text_input_add_char(TextInput* input, char* c);

void text_input_backspace(TextInput* input);

void text_input_return(TextInput* input);

/*
 * Returns true if the text input is being clicked.
 */
bool text_input_update(TextInput* input);

#endif
