#ifndef PLAYER_LIST_H
#define PLAYER_LIST_H

typedef struct PlayerArray PlayerArray;

#include "player.h"

typedef struct PlayerArrayCell {
	bool exists;
	Player player;
} PlayerArrayCell;

// Dynamic Array for Player
struct PlayerArray {
	PlayerArrayCell* data;
	Uint32 len;
	Uint32 count;

	Uint32 cap;
};

// Add a player to the list. It will be given the smallest unused ID possible.
// The list will be resized if necessary.
// Returns the new player ID.
int player_array_get_next_unused_id(const PlayerArray* list);

// Insert a new player with the given ID.
// Returns a pointer to this player.
Player* player_array_insert(PlayerArray* list, uint id);

// Remove a player from the list, leaving an empty slot to be reused
// later on.
void player_array_remove(PlayerArray* list, int i);

// Initialize a player list with an initial capacity.
// It will be resized automatically.
// The list must be freed with `PlayerArray_free`.
void player_array_init(PlayerArray* list, int capacity);

// Empty the player list and free the associated memory.
void player_array_free(PlayerArray* list);

#endif
