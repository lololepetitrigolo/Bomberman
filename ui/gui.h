#ifndef UI_H
#define UI_H

#include "ui/button_list.h"
#include "ui/text_input.h"
#include <SDL_events.h>

typedef struct Gui {
	ButtonList* buttons;
	TextInput* text_inputs;
	uint text_inputs_len;
	TextInput* focused_text_input;
} Gui;

void gui_init(void);

void gui_quit(void);

void gui_update(void);

void gui_draw(SDL_Renderer* renderer);

/* Delete all UI elements */
void gui_clear(void);

void gui_handle_text_input_event(SDL_TextInputEvent event);
void gui_handle_keyboard_event(SDL_KeyboardEvent event);
void gui_handle_button_up_event(SDL_MouseButtonEvent event);

void gui_set_focused_text_input(TextInput* input);

Button* gui_create_button(SDL_Rect rect,
						  ButtonCallbackFn on_click,
						  void* on_click_data,
						  char* text,
						  TTF_Font* font);

void gui_destroy_button(Button* button);

TextInput* gui_create_text_input(char* text,
								 SDL_Rect position,
								 SDL_Color background_color,
								 SDL_Color text_color,
								 TTF_Font* font,
								 int font_size);

void gui_destroy_text_input(TextInput* text_input);

#endif
