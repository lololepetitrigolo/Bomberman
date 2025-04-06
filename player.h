#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <SDL_render.h>
#include <stdbool.h>

#include "shared/common.h"

#include "grid.h"
typedef struct Player Player;

#define NB_MAX_POWERUPS 20

#include "powerup.h"

typedef enum Skin {
	SKIN_RECT_COLOR,
	SKIN_IMAGE,
} SkinType;

struct Player {
	bool is_alive;

	char name[PLAYER_NAME_LENGTH];
	PlayerId id;

	float x;
	float y;
	float orientation;

	float size;
	SkinType skin;
	SDL_Texture* texture; // is NULL if skin == SKIN_RECT_COLOR

	float speed;
	int range;

	// Timers
	Uint64 time_last_bomb; // Last time a bomb was placed in milliseconds
	Uint64 teleporter_time;
	Uint64 most_recent_update;

	PowerupState* powerups;
	int nb_powerup;
};

void player_init(Player* player, PlayerId id, SDL_Texture* texture);

void player_set_name(Player* player, const char* name);

// Move the player by (dx, dy) if possible
void player_move(Player* player, Grid* grid, float dx, float dy);

int player_orient(float dx, float dy);

#endif
