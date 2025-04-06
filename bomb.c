#include "bomb.h"
#include "game.h"
#include "grid.h"
#include "ice_network.h"
#include "message.h"
#include "powerup.h"
#include <SDL.h>

Bomb* find_bomb(Game* game, int i, int j) {
	for (int k = 0; k < (int)game->bombs->len; k++) {
		int j_b = (int)game->bombs->data[k].x / game->grid->cell_size;
		int i_b = (int)game->bombs->data[k].y / game->grid->cell_size;
		if (i == i_b && j == j_b) {
			return &game->bombs->data[k];
		}
	}
	printf("Bomb not found in function find_bomb\n");
	return NULL;
}

void add_random_powerup(Game* game, int i, int j) {
	// add a power_up with the proportion 25% (and create the packet to transmit
	// the apparition of a powerup)

	if (game->local_player->id == (int)((i + j) % game->players.len)) {
		int id_first_power_up = CELL_TYPE_POWERUP;
		int nbr_powerups = END_POWER_UPS - id_first_power_up + 1;

		// the oroba can be changed
		int proba = 15;

		int random_number = rand() % (nbr_powerups * (100 / proba));
		if (random_number < nbr_powerups) {
			Powerup powerup = random_number + id_first_power_up;
			game->grid->board[i][j] = (CellType)powerup;

			Message message;
			message.type = MESSAGE_TYPE_UPDATE_POWERUP;
			message.player_id = game->local_player->id;
			message.update_powerup.is_taken = false;
			message.update_powerup.powerup_i = i;
			message.update_powerup.powerup_j = j;
			message.update_powerup.powerup_type = powerup;
			message.update_powerup.time = get_time();
			broadcast_message(&message);
		}
	}
}

void killings_cells(Game* game, Bomb* bomb, bool is_killing) {
	Grid* grid = game->grid;
	int i_b = (int)(bomb->y / grid->cell_size);
	int j_b = (int)(bomb->x / grid->cell_size);
	CellType cell_type_a = (is_killing) ? CELL_TYPE_KILLING : CELL_TYPE_EMPTY;

	Uint64 time = get_time();
	grid->board[i_b][j_b] = cell_type_a;
	for (int i = 1; i <= bomb->range; i++) {
		if (i_b + i < grid->height - 1) {
			if (grid_cell_is_good(grid->board[i_b + i][j_b])) {
				grid->board[i_b + i][j_b] = cell_type_a;
				if (!is_killing)
					add_random_powerup(game, i_b + i, j_b);
			} else if (grid->board[i_b + i][j_b] == CELL_TYPE_BOMB) {
				Bomb* bomb = find_bomb(game, i_b + i, j_b);
				if (bomb != NULL && bomb->blast_time > time) {
					bomb->blast_time = time;
					bomb->blast_end = time + 500000;
				}
			} else
				break;
		} else
			break;
	}
	for (int i = 1; i <= bomb->range; i++) {
		if (j_b + i < grid->width + 1) {
			if (grid_cell_is_good(grid->board[i_b][j_b + i])) {
				grid->board[i_b][j_b + i] = cell_type_a;
				if (!is_killing)
					add_random_powerup(game, i_b, j_b + i);
			} else if (grid->board[i_b][j_b + i] == CELL_TYPE_BOMB) {
				Bomb* bomb = find_bomb(game, i_b, j_b + i);
				if (bomb != NULL && bomb->blast_time > time) {
					bomb->blast_time = time;
					bomb->blast_end = time + 500000;
				}
			} else
				break;
		} else
			break;
	}
	for (int i = 1; i <= bomb->range; i++) {
		if (i_b > 0) {
			if (grid_cell_is_good(grid->board[i_b - i][j_b])) {
				grid->board[i_b - i][j_b] = cell_type_a;
				if (!is_killing)
					add_random_powerup(game, i_b - i, j_b);
			} else if (grid->board[i_b - i][j_b] == CELL_TYPE_BOMB) {
				Bomb* bomb = find_bomb(game, i_b - i, j_b);
				if (bomb != NULL && bomb->blast_time > time) {
					bomb->blast_time = time;
					bomb->blast_end = time + 500000;
				}
			} else
				break;
		} else
			break;
	}
	for (int i = 1; i <= bomb->range; i++) {
		if (j_b - i > 0) {
			if (grid_cell_is_good(grid->board[i_b][j_b - i])) {
				grid->board[i_b][j_b - i] = cell_type_a;
				if (!is_killing)
					add_random_powerup(game, i_b, j_b - i);
			} else if (grid->board[i_b][j_b - i] == CELL_TYPE_BOMB) {
				Bomb* bomb = find_bomb(game, i_b, j_b - i);
				if (bomb != NULL && bomb->blast_time > time) {
					bomb->blast_time = time;
					bomb->blast_end = time + 500000;
				}
			} else
				break;
		} else
			break;
	}
}
