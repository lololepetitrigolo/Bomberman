#ifndef BOMB_H
#define BOMB_H

typedef struct Bomb Bomb;
typedef struct BombStats BombStats;

#include "player.h"
#include <stdbool.h>

struct BombStats {
	int itself_blast_time;
	int itself_range;
};

// A bomb placed on the map
struct Bomb {
	float x;
	float y;
	PlayerId player;
	// Global begin time of the explosion
	Uint64 blast_time;
	// Global ending time of the explosion
	Uint64 blast_end;
	int range;
};

#include "game.h"
void killings_cells(Game* game, Bomb* bomb, bool is_killing);

#endif
