#ifndef CLIENT_MESSAGE_H
#define CLIENT_MESSAGE_H

#include "juice/juice.h"
#include "shared/common.h"

typedef enum ClientMessageType {
	CLIENT_GOOBYE,
	CREATE_ROOM,
	/* A request from the player to see the existing games */
	GET_ROOMS,
	JOIN_ROOM,
	SEND_AGENT_TO_SERVER,
} ClientMessageType;

/* Send by the host to the server to create a room */
typedef struct CreateRoomRequest {
	char room_name[ROOM_NAME_LENGTH];
	char player_name[PLAYER_NAME_LENGTH];
	uint seed;
	uint max_players;
} CreateRoomRequest;

/* Send by a player who want to join a game */
typedef struct JoinRoom {
	char room_name[32];
	char player_name[32];
} JoinRoom;

/* The player create localy an agent and send the information to connect with
 * this agent */
typedef struct SendAgentToServer {
	PlayerId from_player; // id of the player who send the agent
	PlayerId to_player;	  // id of the player who must receive the agent
	RoomId room_id;
	char ice_sdp[JUICE_MAX_SDP_STRING_LEN];
} SendAgentToServer;

/* Message from a client to the server */
typedef struct ClientMessage {
	ClientMessageType type;
	union {
		CreateRoomRequest create_room;
		JoinRoom join_room;
		SendAgentToServer send_agent_to_server;
	};
} ClientMessage;

#endif
