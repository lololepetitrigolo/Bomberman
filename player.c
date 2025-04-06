#include <SDL_image.h>
#include <SDL_render.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "collision.h"
#include "game.h"
#include "math.h"
#include "player.h"
#include "shared/common.h"

void player_init(Player* player, PlayerId id, SDL_Texture* texture) {
	player->time_last_bomb = 0;
	snprintf(player->name, PLAYER_NAME_LENGTH, "Player %ud", id);
	player->id = id;
	player->speed = 0.1;
	player->range = 0;
	player->size = .5;
	player->skin = SKIN_IMAGE;
	player->texture = texture;
	player->is_alive = true;
	player->most_recent_update = 0;
	player->orientation = 0;
	player->powerups =
		(PowerupState*)malloc(NB_MAX_POWERUPS * sizeof(PowerupState));
	player->nb_powerup = 0;
	player->teleporter_time = 0;
}

void player_set_name(Player* player, const char* name) {
	strncpy(player->name, name, PLAYER_NAME_LENGTH);
}

inline static int max(int a, int b) { return a > b ? a : b; }
inline static int min(int a, int b) { return a < b ? a : b; }

static bool is_move_legal(Player* player, Grid* grid, float dx, float dy) {
	assert(grid);
	assert(player);

	float player_size = player->size * grid->cell_size;

	SDL_FRect future_rect = {player->x - player_size / 2 + dx,
							 player->y - player_size / 2 + dy,
							 player_size,
							 player_size};

	// Find the coordonates of the cell of the player
	int i_p = (int)floor(player->y / grid->cell_size);
	int j_p = (int)floor(player->x / grid->cell_size);

	int i0 = i_p;
	int i1 = i_p;
	int j0 = j_p;
	int j1 = j_p;

	if (dx < 0) {
		i0 = i_p - 1;
		i1 = i_p + 1;
		j0 = j_p - 1;
		j1 = j0;
	} else if (dx > 0) {
		i0 = i_p - 1;
		i1 = i_p + 1;
		j0 = j_p + 1;
		j1 = j0;
	} else if (dy < 0) {
		j0 = j_p - 1;
		j1 = j_p + 1;
		i0 = i_p - 1;
		i1 = i0;
	} else if (dy > 0) {
		j0 = j_p - 1;
		j1 = j_p + 1;
		i0 = i_p + 1;
		i1 = i0;
	} else {
		// fprintf(stderr, "Error in is_move_legal: dx or dy should be != 0\n");
	}

	i0 = min(max(i0, 0), grid->height - 1);
	j0 = min(max(j0, 0), grid->width - 1);
	i1 = min(max(i1, 0), grid->width - 1);
	j1 = min(max(j1, 0), grid->width - 1);

	for (int i = i0; i <= i1; i++) {
		for (int j = j0; j <= j1; j++) {
			SDL_FRect cell = {j * grid->cell_size,
							  i * grid->cell_size,
							  grid->cell_size,
							  grid->cell_size};

			if (grid->board[i][j] < CELL_TYPE_EMPTY &&
				rect_intersects(&cell, &future_rect)) {
				return false;
			}
		}
	}

	return true;
}

int player_orient(float dx, float dy) {
	int quarter = 45; // the degree of a quarter of a round
	int orientation = 0;

	// 8 cases of the orientation

	if (dx == -1 && dy == -1) {
		orientation = 3 * quarter;
	}

	else if (dx == -1 && dy == 0) {
		orientation = 2 * quarter;
	}

	else if (dx == -1 && dy == 1) {
		orientation = 1 * quarter;
	}

	else if (dx == 0 && dy == -1) {
		orientation = 4 * quarter;
	}

	else if (dx == 0 && dy == 1) {
		orientation = 0 * quarter;
	}

	else if (dx == 1 && dy == -1) {
		orientation = -3 * quarter;
	}

	else if (dx == 1 && dy == 0) {
		orientation = -2 * quarter;
	}

	else if (dx == 1 && dy == 1) {
		orientation = -1 * quarter;
	}

	return orientation;
}

void player_move(Player* player, Grid* grid, float dx, float dy) {
	// change the orientation of the player
	player->orientation = player_orient(dx, dy);
	// Normalize the movement
	float move_length = sqrt(dx * dx + dy * dy);
	dx = dx / move_length;
	dy = dy / move_length;

	// Scale by the player's speed
	dx *= player->speed * grid->cell_size;
	dy *= player->speed * grid->cell_size;

	// semi length of the player
	int semi_length = (int)(player->size * grid->cell_size) / 2;

	// find the coordonate of the cell of the player
	int i_p = (int)floor(player->y / grid->cell_size);
	int j_p = (int)floor(player->x / grid->cell_size);

	// the player would possibly like to move inside an UNBREAKABLE or inside a
	// KILLING
	if (is_move_legal(player, grid, dx, 0)) {
		player->x += dx;
	} else {
		if (dx > 0) {
			player->x = (j_p + 1) * grid->cell_size - 2 - semi_length;
		} else {
			player->x = (j_p)*grid->cell_size + 2 + semi_length;
		}
	}
	if (is_move_legal(player, grid, 0, dy)) {
		player->y += dy;
	} else {
		if (dy > 0) {
			player->y = (i_p + 1) * grid->cell_size - 2 - semi_length;
		} else {
			player->y = (i_p)*grid->cell_size + 2 + semi_length;
		}
	}
}
