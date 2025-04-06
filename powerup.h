#ifndef POWERUP_H
#define POWERUP_H

#include "grid.h"
#include <SDL.h>

typedef enum {
	POWERUP_SPEED = CELL_TYPE_POWERUP,
	POWERUP_BOMB_RANGE,
	// keep the END_POWER_UPS on the end of the enum type
	END_POWER_UPS,
} Powerup;

typedef struct {
	Powerup type;
	Uint64 end_time;
} PowerupState;

#include "player.h"

// add a powerup on the grid
void remove_powerup(Grid* grid, int i, int j);

// remove a powerup from the grid
void add_powerup(Grid* grid, int i, int j, Powerup powerup_type);

// check if the player is on a powerup
Powerup check_powerup(Grid* grid, Player* player);

// check if the player as a powerup that must be finished
void check_powerup_end(Player* player);

// apply the effect to the player
void add_powerup_to_player(Player* player, Powerup powerup_type);

// Returns the powerup's name as a string
const char* get_powerup_name(Powerup powerup_type);

#endif
