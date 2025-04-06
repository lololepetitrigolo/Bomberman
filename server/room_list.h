#ifndef SERVER_ROOM_LIST
#define SERVER_ROOM_LIST

#include <stdbool.h>
#include <stdlib.h>

#include "room.h"

typedef struct RoomList {
	size_t cap;
	size_t len;
	Room* rooms;
} RoomList;

void RoomList_Init(RoomList* list, size_t capacity);

bool RoomList_IsFull(RoomList* list);

RoomId RoomList_Push(RoomList* list, Room room);

Room* RoomList_FindByName(RoomList* list, const char* name);

#endif
