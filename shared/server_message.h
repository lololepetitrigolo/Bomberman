#ifndef SERVER_MESSAGE_H
#define SERVER_MESSAGE_H

#include "shared/common.h"

#define NB_MAX_GAME 20
#define NB_MAX_PLAYERS 10

typedef enum ServerMessageType {
	/* A hello sent to new connections */
	SERVER_HELLO,
	/* Response to a client who wants to create a room. */
	CREATE_ROOM_RESPONSE,
	/* Response to a client who wants to join a room. */
	JOIN_ROOM_RESPONSE,
	/* Response to a client who wants to see the list of public rooms. */
	SEND_ROOMS,
	/* Request a client to send their p2p agent information. */
	GET_AGENT,
	/* Inform a room member that a new player has joined their room. */
	ADD_PLAYER_IN_ROOM,
	/* Transmit p2p agent information of a player to another */
	SEND_AGENT_TO_PLAYER,
} ServerMessageType;

typedef enum CreateRoomResponseStatus {
	CREATE_ROOM_RESPONSE_OK,
	CREATE_ROOM_RESPONSE_TOO_MUCH_ROOMS,
	CREATE_ROOM_RESPONSE_NAME_ALREADY_USED,
} CreateRoomResponseStatus;

/* 
 * Sent by the server to a player who requested to create a room.
 * When `status` is not OK, the other fields must be ignored.
 * When `status` is OK, the player is considered in the newly created room.
 */
typedef struct CreateRoomResponse {
	/* Status indicating success, or the reason for failure. */
	CreateRoomResponseStatus status;
	/* Unique ID of the newly created room. */
	RoomId room_id;
} CreateRoomResponse;

/* This message is send to a player who want to see the game wich exist */
typedef struct SendRooms {
	Uint32 room_count;
	RoomInfo rooms[NB_MAX_GAME];
} SendRooms;

typedef enum JoinRoomResponseStatus {
		JOIN_ROOM_RESPONSE_OK,
		JOIN_ROOM_RESPONSE_NAME_ALREADY_USED,
		JOIN_ROOM_RESPONSE_INVALID_ROOM,
		JOIN_ROOM_RESPONSE_ROOM_IS_FULL,
} JoinRoomResponseStatus;

/* Response to a JoinRoom request */
typedef struct JoinRoomResponse {
	JoinRoomResponseStatus status;
	char room_name[ROOM_NAME_LENGTH];
	RoomId room_id;

	// The following fields are set only if the response is OK:

	// The ID assigned by the server to the joining player.
	PlayerId player_id;
	// Amount of players already present in the room.
	int nb_player;
	// IDs of players already in the room.
	PlayerId players_id[NB_MAX_PLAYERS];
	// Names of players already in the room.
	char players_name[PLAYER_NAME_LENGTH][NB_MAX_PLAYERS];
	// Skins of players already in the room.
	int players_skin[NB_MAX_PLAYERS];
	// Seed used for random number generation.
	int game_seed;
} JoinRoomResponse;

/*
 * Sent by the server to all players of a room, to tell them that a new
 * player has joined the room.
 */
typedef struct AddPlayerInRoom {
	PlayerId player_id;
	char player_name[PLAYER_NAME_LENGTH];
} AddPlayerInRoom;

/* A request of the seerver to player to have an agent
 * This message is send to each player in the game when someone join the game */
typedef struct GetAgent {
	PlayerId player_id; // Id of the player who want to conect with you
	char player_name[PLAYER_NAME_LENGTH];
	int player_skin;
} GetAgent;

/* The server use this message to complete a connection beetween two player
 * The server send this message to the player with the id given in
 * SendAgentToServer */
typedef struct SendAgentToPlayer {
	PlayerId from_player; // id of the player who send the agent to the server
	PlayerId to_player;	  // id of the player who must receive this message
	char ice_sdp[ICE_MESSAGE_SIZE];
} SendAgentToPlayer;

/* Message from the server to a client */
typedef struct ServerMessage {
	ServerMessageType type;
	union {
		CreateRoomResponse create_room_response;
		JoinRoomResponse join_room_response;
		SendRooms send_rooms;
		AddPlayerInRoom add_player_in_room;
		GetAgent get_agent;
		SendAgentToPlayer send_agent_to_player;
	};
} ServerMessage;

#endif
