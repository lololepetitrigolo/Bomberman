#include <SDL_ttf.h>

#include "debug.h"
#include "string.h"

#define DEBUG_DRAW_LINE_LENGTH 256

static const char* font_path = "assets/Hack-Regular.ttf";

typedef struct DebugTextQueue {
	size_t len;
	size_t cap;
	char** txts;
} DebugTextQueue;

static DebugTextQueue queue;
static TTF_Font* font = NULL;

static void debug_text_queue_init(DebugTextQueue* q) {
	q->len = 0;
	q->cap = 4;
	q->txts = malloc(q->cap * sizeof(*q->txts));
}

static void debug_text_queue_finish(DebugTextQueue* q) {
	q->len = 0;
	q->cap = 0;
	free(q->txts);
}

static void debug_text_queue_push(DebugTextQueue* q, char* txt) {
	if (q->len == q->cap) {
		q->cap *= 2;
		q->txts = realloc(q->txts, q->cap * sizeof(*q->txts));
	}

	q->txts[q->len] = txt;
	q->len++;
}

void debug_init(void) {
	debug_text_queue_init(&queue);
	font = TTF_OpenFont(font_path, 12);
	if (!font) {
		SDL_LogError(0,
					 "debug_init: Cannot load font %s: %s",
					 font_path,
					 TTF_GetError());
	}
}

void debug_quit(void) {
	debug_text_queue_finish(&queue);
	TTF_CloseFont(font);
}

void screen_printf(const char* format, ...) {
	va_list args;
	va_start(args, format);

	char* buf = malloc(DEBUG_DRAW_LINE_LENGTH);
	vsprintf(buf, format, args);
	debug_text_queue_push(&queue, buf);

	va_end(args);
}

void debug_draw(SDL_Renderer* renderer) {
	SDL_Rect dest = {0, 0, 0, 0};
	for (size_t i = 0; i < queue.len; i++) {
		SDL_Color color = {0xFF, 0xFF, 0xFF, 0xFF};
		SDL_Surface* surface = TTF_RenderUTF8_Solid(font, queue.txts[i], color);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
		SDL_RenderCopy(renderer, texture, NULL, &dest);
		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);
		free(queue.txts[i]);
		dest.y += dest.h;
	}
	queue.len = 0;
}
