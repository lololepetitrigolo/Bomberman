#include "room_list.h"

#include <assert.h>

void RoomList_Init(RoomList* list, size_t capacity) {
	assert(capacity > 0);
	list->cap = capacity;
	list->len = 0;
	list->rooms = malloc(capacity * sizeof(*list->rooms));
}

inline bool RoomList_IsFull(RoomList* list) { return list->len == list->cap; }

RoomId RoomList_Push(RoomList* list, Room room) {
	if (RoomList_IsFull(list)) {
		return -1;
	}
	room.room_id = list->len;
	list->rooms[list->len] = room;
	list->len++;
	return list->len - 1;
}

Room* RoomList_FindByName(RoomList* list, const char* name) {
	assert(list);
	for (size_t i = 0; i < list->len; i++) {
		if (!strcmp(list->rooms[i].name, name)) {
			return &list->rooms[i];
		}
	}
	return NULL;
}
