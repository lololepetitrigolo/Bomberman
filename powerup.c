#include "powerup.h"
#include "game.h"
#include "grid.h"
#include <assert.h>
#include <sys/time.h>

#define time_speed_powerup 2 * second

void powerup_increase_range(Player* player) { player->range += 1; }

void powerup_increase_speed(Player* player) { player->speed *= 2; }

void remove_powerup(Grid* grid, int i, int j) {
	grid->board[i][j] = CELL_TYPE_EMPTY;
}

void add_powerup(Grid* grid, int i, int j, Powerup powerup_type) {
	grid->board[i][j] = (CellType)powerup_type;
}

void add_powerup_to_player(Player* player, Powerup powerup_type) {
	// choice the function to appply according to the powerup
	if (powerup_type == POWERUP_SPEED) {
		powerup_increase_speed(player);
	} else if (powerup_type == POWERUP_BOMB_RANGE) {
		powerup_increase_range(player);
	}
	Uint64 time = get_time();

	player->powerups[player->nb_powerup] =
		(PowerupState){powerup_type, time + 2 * 1000000};
	player->nb_powerup++;
}

Powerup check_powerup(Grid* grid, Player* player) {
	int i = (int)player->y / grid->cell_size;
	int j = (int)player->x / grid->cell_size;

	if (grid->board[i][j] >= CELL_TYPE_POWERUP) {
		assert(player->nb_powerup != NB_MAX_POWERUPS);

		Powerup powerup_type = (Powerup)grid->board[i][j];
		add_powerup_to_player(player, powerup_type);

		remove_powerup(grid, i, j);
		return powerup_type;
	}
	return 0;
}

void check_powerup_end(Player* player) {
	Uint64 time = get_time();
	for (int i = 0; i < player->nb_powerup; i++) {
		if (player->powerups[i].end_time < time) {
			Powerup powerup_type = player->powerups[i].type;

			if (powerup_type == POWERUP_SPEED) {
				player->speed /= 2;
			} else if (powerup_type == POWERUP_BOMB_RANGE) {
				player->range = 0;
			}

			player->powerups[i] = player->powerups[player->nb_powerup - 1];
			player->nb_powerup -= 1;
		}
	}
}

static const char* powerup_names[] = {
	[POWERUP_SPEED - CELL_TYPE_POWERUP] = "Speed+\0",
	[POWERUP_BOMB_RANGE - CELL_TYPE_POWERUP] = "Bomb range+\0",
};

const char* get_powerup_name(Powerup powerup_type) {
	return powerup_names[powerup_type - CELL_TYPE_POWERUP];
}
