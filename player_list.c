#include "player_list.h"

void player_array_init(PlayerArray* list, int capacity) {
	list->cap = capacity;
	list->len = 0;
	list->count = 0;
	list->data = (PlayerArrayCell*)calloc(capacity, sizeof(PlayerArrayCell));
}

int player_array_get_next_unused_id(const PlayerArray* list) {
	uint i;
	for (i = 0; i < list->len; i++) {
		if (!list->data[i].exists) {
			break;
		}
	}
	return i;
}

Player* player_array_insert(PlayerArray* list, uint id) {
	// Grow the list if necessary
	if (id >= list->cap) {
		if (list->cap == list->len) {
			list->cap *= 2;
			list->data = realloc(list->data, list->cap * sizeof(PlayerArrayCell));
		}
	}
	
	if (id >= list->len) {
		list->len = id + 1;
	}

	list->data[id].exists = true;
	list->data[id].player.id = id;

	list->count += 1;

	return &list->data[id].player;
}

void player_array_remove(PlayerArray* list, int i) {
	list->data[i].exists = false;
	list->count -= 1;
}

void player_array_free(PlayerArray* list) {
	free(list->data);
	list->len = 0;
	list->count = 0;
	list->cap = 0;
}
