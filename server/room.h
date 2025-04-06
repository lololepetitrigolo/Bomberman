#ifndef SERVER_ROOM_H
#define SERVER_ROOM_H

#include <stdbool.h>

#include "client_list.h"
#include "shared/common.h"

#define ROOM_MAX_PLAYERS 8

typedef struct RoomPlayer {
	char name[PLAYER_NAME_LENGTH];
	TCPsocket sock;
	PlayerId id;
	int skin;
	struct RoomPlayer* next;
} RoomPlayer;

typedef struct RoomPlayerList {
	RoomPlayer* head;
	RoomPlayer* tail;
	uint len;
} RoomPlayerList;

typedef struct Room {
	char name[ROOM_NAME_LENGTH];
	RoomPlayerList players;
	int seed;
	uint max_players;
	bool used_id[ROOM_MAX_PLAYERS];
	RoomId room_id;
} Room;

void Room_Init(Room* room, uint max_players);

// TODO: Room_Destroy

bool Room_HasPlayerName(const Room* room, const char* name);

PlayerId Room_GetUnusedId(const Room* room);

void Room_InsertPlayer(Room* room,
					   PlayerId id,
					   const char* name,
					   TCPsocket sock);

bool Room_IsFull(const Room* room);

void Room_Broadcast(Room* room, const void* data, size_t len);

#endif
