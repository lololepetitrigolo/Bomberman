#ifndef SHARED_COMMON_H
#define SHARED_COMMON_H

#include <SDL.h>

#define ROOM_NAME_LENGTH 32
#define PLAYER_NAME_LENGTH 32

#define ICE_MESSAGE_SIZE 4096

typedef int PlayerId;
typedef int RoomId;

/* Public information about a game room */
typedef struct RoomInfo {
	char name[ROOM_NAME_LENGTH];
	Uint8 player_count;
	Uint8 size;
} RoomInfo;

#endif
