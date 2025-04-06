#include "ui/gui.h"
#include "ui/button.h"
#include "ui/button_list.h"
#include "ui/text_input.h"

#include <SDL_mouse.h>
#include <assert.h>

const int INITIAL_BUTTON_LIST_CAPACITY = 8;
const int INITIAL_TEXT_INPUT_LIST_CAPACITY = 4;

static Gui gui;

void gui_init(void) {
	gui.buttons = button_list_create(INITIAL_BUTTON_LIST_CAPACITY);
	gui.text_inputs_len = 0;
	gui.text_inputs =
		calloc(INITIAL_TEXT_INPUT_LIST_CAPACITY, sizeof(TextInput));
	gui.focused_text_input = NULL;
}

void gui_quit(void) {
	button_list_destroy(gui.buttons);
	free(gui.text_inputs);
	gui.focused_text_input = NULL;
}

void gui_update(void) {
	for (uint i = 0; i < gui.buttons->len; i++) {
		button_update(&gui.buttons->buttons[i]);
	}
	for (uint i = 0; i < gui.text_inputs_len; i++) {
		if (text_input_update(&gui.text_inputs[i])) {
			gui.focused_text_input = &gui.text_inputs[i];
		}
	}
}

void gui_draw(SDL_Renderer* renderer) {
	for (uint i = 0; i < gui.buttons->len; i++) {
		button_draw(&gui.buttons->buttons[i], renderer);
	}
	for (uint i = 0; i < gui.text_inputs_len; i++) {
		text_input_draw(&gui.text_inputs[i], renderer);
	}
}

Button* gui_create_button(SDL_Rect rect,
						  ButtonCallbackFn on_click,
						  void* on_click_data,
						  char* text,
						  TTF_Font* font) {
	Button* button = button_list_new_button(gui.buttons);
	button_init(button, rect, on_click, on_click_data, text, font);
	return button;
}

TextInput* gui_create_text_input(char* text,
								 SDL_Rect position,
								 SDL_Color background_color,
								 SDL_Color text_color,
								 TTF_Font* font,
								 int font_size) {
	if (gui.text_inputs_len == INITIAL_TEXT_INPUT_LIST_CAPACITY) {
		SDL_LogError(0, "Cannot create TextInput: max capacity is reached");
		return NULL;
	}

	gui.text_inputs_len += 1;

	text_input_init(&gui.text_inputs[gui.text_inputs_len - 1],
					text,
					position,
					background_color,
					text_color,
					font,
					font_size);

	return &gui.text_inputs[gui.text_inputs_len - 1];
}

void gui_destroy_button(Button* button) {
	button_list_remove(gui.buttons, button);
}

void gui_destroy_text_input(TextInput* text_input) {
	assert(text_input);
	size_t index = (text_input - gui.text_inputs) / sizeof(TextInput);
	gui.text_inputs[index] = gui.text_inputs[gui.text_inputs_len - 1];
	gui.text_inputs_len -= 1;
}

void gui_handle_text_input_event(SDL_TextInputEvent event) {
	if (gui.focused_text_input) {
		text_input_add_char(gui.focused_text_input, event.text);
	}
}

void gui_handle_keyboard_event(SDL_KeyboardEvent event) {
	if (gui.focused_text_input) {
		if (event.keysym.sym == SDLK_BACKSPACE) {
			text_input_backspace(gui.focused_text_input);
		} else if (event.keysym.sym == SDLK_RETURN) {
			text_input_return(gui.focused_text_input);
			gui.focused_text_input = NULL;
		}
	}
}

void gui_handle_button_up_event(SDL_MouseButtonEvent event) {
	if (SDL_BUTTON(event.button) == SDL_BUTTON_LEFT) {
		for (uint i = 0; i < gui.buttons->len; i++) {
			Button btn = gui.buttons->buttons[i];
			if (btn.hovered) {
				btn.on_click.fn(btn.on_click.data1, btn.on_click.data2);
			}
		}
	}
}

void gui_clear(void) {
	gui.buttons->len = 0;
	gui.text_inputs_len = 0;
}

void gui_set_focused_text_input(TextInput* input) {
	gui.focused_text_input = input;
}
