#include "graphic.h"
#include "grid.h"
#include "player.h"
#include "powerup.h"
#include <SDL.h>
#include <SDL_image.h>

void draw_grid(SDL_Renderer* renderer, Grid* grid) {
	int window_width;
	int window_height;
	SDL_GetRendererOutputSize(renderer, &window_width, &window_height);

	for (int i = 0; i < grid->height; i++) {
		for (int j = 0; j < grid->width; j++) {
			SDL_Rect wall = {j * grid->cell_size + window_width / 2 -
								 grid->cell_size / 2 * grid->width + 1,
							 i * grid->cell_size + window_height / 2 -
								 (grid->cell_size / 2) * grid->height + 1,
							 grid->cell_size - 2,
							 grid->cell_size - 2};

			if (grid->board[i][j] == CELL_TYPE_UNBREAKABLE) {
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			} else if (grid->board[i][j] == CELL_TYPE_BREAKABLE) {
				SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
			} else if (grid->board[i][j] == CELL_TYPE_BOMB) {
				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
			} else if (grid->board[i][j] == CELL_TYPE_KILLING) {
				SDL_SetRenderDrawColor(renderer, 200, 110, 250, 100);
			} else if (grid->board[i][j] == POWERUP_SPEED) {
				SDL_SetRenderDrawColor(renderer, 0, 60, 250, 100);
			} else if (grid->board[i][j] == POWERUP_BOMB_RANGE) {
				SDL_SetRenderDrawColor(renderer, 90, 40, 0, 100);
			} else if (grid->board[i][j] == CELL_TYPE_TELEPORTER) {
				SDL_SetRenderDrawColor(renderer, 60, 60, 250, 100);
			} else {
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
			}
			SDL_RenderFillRect(renderer, &wall);
		}
	}
}

SDL_Texture* create_player_texture(SDL_Renderer* renderer, char* image_name) {
	SDL_Surface* tmp = NULL;
	SDL_Texture* texture = NULL;
	tmp = IMG_Load(image_name);
	if (tmp == NULL) {
		fprintf(stderr, "Erreur SDL_LoadBMP : %s", SDL_GetError());
	}
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	texture = SDL_CreateTextureFromSurface(renderer, tmp);
	SDL_FreeSurface(tmp); /* On libère la surface, on n’en a plus besoin */
	return texture;
}

void draw_player(SDL_Renderer* renderer, Player* player, Grid* grid) {
	int window_width;
	int window_height;
	SDL_GetRendererOutputSize(renderer, &window_width, &window_height);

	// rect position to draw
	const SDL_FRect rect = {
		player->x - (player->size / 2) * grid->cell_size + window_width / 2 -
			grid->cell_size / 2 * grid->width,
		player->y - (player->size / 2) * grid->cell_size + window_height / 2 -
			grid->cell_size / 2 * grid->height,
		grid->cell_size * player->size,
		grid->cell_size * player->size,
	};

	if (player->skin == SKIN_RECT_COLOR) {
		SDL_SetRenderDrawColor(renderer, 241, 95, 169, 100);
		SDL_RenderFillRectF(renderer, &rect);
	}

	else if (player->skin == SKIN_IMAGE) {
		SDL_RenderCopyExF(renderer,
						  player->texture,
						  NULL,
						  &rect,
						  player->orientation,
						  NULL,
						  SDL_FLIP_NONE);
	}
}
