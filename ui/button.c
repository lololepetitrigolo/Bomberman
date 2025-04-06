#include <SDL_ttf.h>
#include <assert.h>

#include "button.h"

void button_init(Button* button,
				 SDL_Rect rect,
				 ButtonCallbackFn on_click,
				 void* on_click_data,
				 char* text,
				 TTF_Font* font) {
	button->rect = rect;
	button->on_click.fn = on_click;
	button->on_click.data1 = on_click_data;
	button->text = text;
	button->font = font;
	button->hovered = false;
}

static inline bool button_is_hovered(Button* button) {
	return button->hovered;
}

void button_draw(Button* button, SDL_Renderer* renderer) {
	SDL_Rect rect = button->rect;

	// Background
	SDL_Color bg = {0x3d, 0x6a, 0xbb, 0xff};
	if (button_is_hovered(button)) {
		bg.r += 40;
		bg.g += 40;
		bg.b += 40;
	}
	SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
	SDL_RenderFillRect(renderer, &rect);

	// Text
	SDL_Color fg_color = {0xFF, 0xFF, 0xFF, 0xFF};
	SDL_Surface* surface =
		TTF_RenderUTF8_Shaded(button->font, button->text, fg_color, bg);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	assert(texture);
	int text_w, text_h;
	SDL_QueryTexture(texture, NULL, NULL, &text_w, &text_h);
	SDL_Rect text_dest = {
		.x = rect.x + rect.w / 2 - text_w / 2,
		.y = rect.y + rect.h / 2 - text_h / 2,
		.w = text_w,
		.h = text_h,
	};
	SDL_RenderCopy(renderer, texture, NULL, &text_dest);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

void button_update(Button* button) {
	SDL_Rect b = button->rect;
	int x, y;
	SDL_GetMouseState(&x, &y);
	if (b.x <= x && x <= b.x + b.w && b.y <= y && y <= b.y + b.h) {
		button->hovered = true;
	} else {
		button->hovered = false;
	}
}
