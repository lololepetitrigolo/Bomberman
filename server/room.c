#include "room.h"
#include "shared/common.h"

#include <assert.h>

void Room_Init(Room* room, uint max_players) {
	room->max_players = max_players;
	bzero(room->name, ROOM_NAME_LENGTH);
	bzero(room->used_id, ROOM_MAX_PLAYERS);

	room->players.len = 0;
	room->players.head = NULL;
	room->players.tail = NULL;
}

bool Room_HasPlayerName(const Room* room, const char* name) {
	assert(room);
	for (RoomPlayer* p = room->players.head; p != NULL; p = p->next) {
		if (!strcmp(p->name, name)) {
			return true;
		}
	}
	return false;
}

PlayerId Room_GetUnusedId(const Room* room) {
	assert(room);
	assert(!Room_IsFull(room));
	for (uint id = 0; id < room->max_players; id++) {
		if (!room->used_id[id]) {
			return id;
		}
	}
	return -1;
}

void Room_InsertPlayer(Room* room,
					   PlayerId id,
					   const char* name,
					   TCPsocket sock) {
	RoomPlayer* player = malloc(sizeof(*player));
	player->sock = sock;
	player->id = id;
	player->next = NULL;
	strncpy(player->name, name, PLAYER_NAME_LENGTH);

	if (room->players.len == 0) {
		room->players.head = player;
		room->players.tail = player;
	} else {
		room->players.tail->next = player;
		room->players.tail = player;
	}

	assert(!room->used_id[id]);
	room->used_id[id] = true;

	room->players.len++;
}

bool Room_IsFull(const Room* room) {
	assert(room);
	return room->players.len == room->max_players;
}

void Room_Broadcast(Room* room, const void* data, size_t len) {
	for (RoomPlayer* p = room->players.head; p != NULL; p = p->next) {
		SDLNet_TCP_Send(p->sock, data, len);
	}
}
