#include "text_input.h"

void text_input_init(TextInput* input,
					 char* text,
					 SDL_Rect position,
					 SDL_Color background_color,
					 SDL_Color text_color,
					 TTF_Font* font,
					 int font_size) {
	input->text_color = text_color;
	input->background_color = background_color;
	input->font = font;
	input->font_size = font_size;
	input->rect = position;
	input->on_return.fn = NULL;
	input->on_return.data1 = NULL;
	input->on_return.data2 = NULL;

	strncpy(input->text, text, TEXT_INPUT_CAPACITY);
	input->text_len = strnlen(text, TEXT_INPUT_CAPACITY);
	if (input->text_len == TEXT_INPUT_CAPACITY) {
		SDL_LogError(0, "Creating TextInput with an initial text too long");
		input->text[TEXT_INPUT_CAPACITY - 1] = '\0';
	}
}
void text_input_draw(TextInput* input, SDL_Renderer* renderer) {
	int window_width;
	int window_height;
	SDL_GetRendererOutputSize(renderer, &window_width, &window_height);

	// Background
	SDL_SetRenderDrawColor(renderer,
						   input->background_color.r,
						   input->background_color.g,
						   input->background_color.b,
						   input->background_color.a);
	SDL_RenderFillRect(renderer, &input->rect);

	if (input->hovered) {
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		SDL_RenderDrawRect(renderer, &input->rect);
	}

	// Text
	SDL_Rect text_rect = input->rect;
	text_rect.x += 20;
	text_rect.y += 12;
	SDL_Surface* surface = TTF_RenderUTF8_Shaded(
		input->font, input->text, input->text_color, input->background_color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_QueryTexture(texture, NULL, NULL, &text_rect.w, &text_rect.h);
	SDL_RenderCopy(renderer, texture, NULL, &text_rect);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

void text_input_add_char(TextInput* input, char* c) {
	if (strlen(input->text) < 32 && c != NULL) {
		// XXX: is c a single character?
		strcat(input->text, c);
		input->text_len += strlen(c);
	}
}

void text_input_backspace(TextInput* input) {
	if (input->text_len > 0) {
		input->text_len -= 1;
		input->text[input->text_len] = '\0';
	}
}

void text_input_return(TextInput* input) {
	if (input->on_return.fn) {
		input->on_return.fn(input->on_return.data1, input->on_return.data2);
	}
}

bool text_input_update(TextInput* input) {
	SDL_Rect b = input->rect;
	int x, y;
	int mouse = SDL_GetMouseState(&x, &y);
	if (b.x <= x && x <= b.x + b.w && b.y <= y && y <= b.y + b.h) {
		input->hovered = true;
		return (SDL_BUTTON(mouse) == SDL_BUTTON_LEFT);
	} else {
		input->hovered = false;
	}
	return false;
}
