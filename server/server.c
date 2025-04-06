#include "server.h"
#include "menus/join_room_menu.h"
#include "room.h"
#include "room_list.h"
#include "server/client_list.h"
#include "shared/debug.h"
#include "shared/server_message.h"

#include <SDL_net.h>
#include <assert.h>
#include <stdio.h>

int Server_Init(Server* server, ServerOptions options) {
	ClientList_Init(&server->clients, options.max_sockets);
	RoomList_Init(&server->rooms, options.max_rooms);

	if (SDLNet_ResolveHost(&server->ip, NULL, 8888) == -1) {
		SDL_LogError(0, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		return -1;
	}

	server->socket = SDLNet_TCP_Open(&server->ip);
	if (!server->socket) {
		SDL_LogError(0, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		return -1;
	}

	return 0;
}

void Server_Finish(Server* server) {
	ClientList_Finish(&server->clients);
	SDLNet_TCP_Close(server->socket);
	server->socket = 0;
}

void server_handle_create_room_request(Server* server,
									   const CreateRoomRequest req,
									   TCPsocket src) {

	ServerMessage res = {.type = CREATE_ROOM_RESPONSE};

	// Reject the request if there are too much rooms already
	if (RoomList_IsFull(&server->rooms)) {
		res.create_room_response.status = CREATE_ROOM_RESPONSE_TOO_MUCH_ROOMS;
		SDLNet_TCP_Send(src, &res, sizeof(res));
		SDL_LogWarn(0,
					"Rejecting CreateRoom request because there are too much "
					"rooms\n");
		return;
	}

	// Reject the request if a room with the same name already exists
	if (RoomList_FindByName(&server->rooms, req.room_name)) {
		res.create_room_response.status =
			CREATE_ROOM_RESPONSE_NAME_ALREADY_USED;
		SDLNet_TCP_Send(src, &res, sizeof(res));
		SDL_Log("Rejecting CreateRoom request because a room with the same "
				"name exists\n");
		return;
	}

	Room room;
	Room_Init(&room, req.max_players);
	strcpy(room.name, req.room_name);
	room.seed = req.seed;

	Room_InsertPlayer(&room, 0, req.player_name, src);

	RoomId room_id = RoomList_Push(&server->rooms, room);
	res.create_room_response.room_id = room_id;

	res.create_room_response.status = CREATE_ROOM_RESPONSE_OK;
	SDLNet_TCP_Send(src, &res, sizeof(res));

	SDL_Log("Created room [%s] with player {%s}\n", room.name, req.player_name);
}

void server_handle_get_rooms_request(Server* server, TCPsocket src) {
	ServerMessage res = {
		.type = SEND_ROOMS,
		.send_rooms.room_count = server->rooms.len,
	};
	for (size_t i = 0; i < server->rooms.len; i++) {
		strcpy(res.send_rooms.rooms[i].name, server->rooms.rooms[i].name);
		res.send_rooms.rooms[i].player_count =
			server->rooms.rooms[i].players.len;
		res.send_rooms.rooms[i].size = server->rooms.rooms[i].max_players;
	}
	int msg_len = sizeof(res.type) + sizeof(res.send_rooms.room_count) +
				  server->rooms.len * sizeof(res.send_rooms.rooms[0]);
	SDLNet_TCP_Send(src, &res, msg_len);
	SDL_Log("Sent rooms (%ld)", server->rooms.len);
}

void server_handle_join_room_request(Server* server,
									 const JoinRoom req,
									 TCPsocket src) {
	ServerMessage msg = {.type = JOIN_ROOM_RESPONSE};

	// Size of the message when we only need to send the status,
	// i.e. in case of a rejection.
	const int status_only_message_size =
		sizeof(msg.type) + sizeof(msg.join_room_response.status) +
		sizeof(msg.join_room_response.room_id) +
		sizeof(msg.join_room_response.room_name);

	strncpy(msg.join_room_response.room_name, req.room_name, ROOM_NAME_LENGTH);

	Room* room = RoomList_FindByName(&server->rooms, req.room_name);

	// Check if the room exists.
	if (!room) {
		msg.join_room_response.status = JOIN_ROOM_RESPONSE_INVALID_ROOM;
		SDLNet_TCP_Send(src, &msg, status_only_message_size);
		SDL_Log("JoinRoom rejected: invalid room [%s]", req.room_name);
		return;
	}

	// Reject the request if the room is already full.
	if (Room_IsFull(room)) {
		msg.join_room_response.status = JOIN_ROOM_RESPONSE_ROOM_IS_FULL;
		SDLNet_TCP_Send(src, &msg, status_only_message_size);
		SDL_Log("JoinRoom rejected: room is full");
		return;
	}

	// Reject the request if the player name is already used.
	if (Room_HasPlayerName(room, req.player_name)) {
		msg.join_room_response.status = JOIN_ROOM_RESPONSE_NAME_ALREADY_USED;
		SDLNet_TCP_Send(src, &msg, status_only_message_size);
		SDL_Log("JoinRoom rejected: name {%s} already used", req.player_name);
		return;
	}

	PlayerId id = Room_GetUnusedId(room);

	// Tell the players in the room that someone has joined.
	ServerMessage message = {
		.type = ADD_PLAYER_IN_ROOM,
		.add_player_in_room.player_id = id,
	};
	strcpy(message.add_player_in_room.player_name, req.player_name);
	Room_Broadcast(room, &message, sizeof(message));

	// Tell the player that he could join, and send information
	// about the other players.
	msg.join_room_response.status = JOIN_ROOM_RESPONSE_OK;
	msg.join_room_response.game_seed = room->seed;
	msg.join_room_response.nb_player = room->players.len;
	msg.join_room_response.player_id = id;
	msg.join_room_response.room_id = room->room_id;

	uint i = 0;
	for (RoomPlayer* p = room->players.head; p != NULL; p = p->next) {
		assert(p->id >= 0);
		msg.join_room_response.players_id[i] = p->id;
		msg.join_room_response.players_skin[i] = p->skin;
		strcpy(msg.join_room_response.players_name[i], p->name);
		i++;
	}

	// TODO: Compute the size of the message based on the amount
	// of players in the room? (avoids sending useless padding)
	SDLNet_TCP_Send(
		src, &msg, sizeof(msg.type) + sizeof(msg.join_room_response));

	// Insert the player in the room
	Room_InsertPlayer(room, id, req.player_name, src);

	SDL_Log("Player {%s} joined room [%s] with ID %d",
			req.player_name,
			room->name,
			id);

	ServerMessage get_agent_msg = {.type = GET_AGENT};
	get_agent_msg.get_agent.player_id = id;
	strcpy(get_agent_msg.get_agent.player_name, req.player_name);
	Room_Broadcast(room, &get_agent_msg, sizeof(get_agent_msg));
}

void Server_HandleMessage(Server* server,
						  const ClientMessage* msg,
						  TCPsocket src) {
	switch (msg->type) {

	case CLIENT_GOOBYE:
		// TODO: if the client is in a room, remove it from the room
		// and tell the other players
		printf("Closed connection from ");
		print_IPaddress(*SDLNet_TCP_GetPeerAddress(src));
		printf("\n");
		ClientList_RemoveBySocket(&server->clients, src);
		break;

	case CREATE_ROOM:
		server_handle_create_room_request(server, msg->create_room, src);
		break;

	case GET_ROOMS:
		server_handle_get_rooms_request(server, src);
		break;

	case JOIN_ROOM:
		server_handle_join_room_request(server, msg->join_room, src);
		break;

	case SEND_AGENT_TO_SERVER: {
		SDL_Log("Received sdp from player : %d, and sending to player : %d\n",
				msg->send_agent_to_server.from_player,
				msg->send_agent_to_server.to_player);
		ServerMessage message;
		message.type = SEND_AGENT_TO_PLAYER;
		message.send_agent_to_player.from_player =
			msg->send_agent_to_server.from_player;
		message.send_agent_to_player.to_player =
			msg->send_agent_to_server.to_player;
		memcpy(message.send_agent_to_player.ice_sdp,
			   msg->send_agent_to_server.ice_sdp,
			   JUICE_MAX_SDP_STRING_LEN);
		// Get the socket of the player to response
		RoomPlayer* cell =
			server->rooms.rooms[msg->send_agent_to_server.room_id].players.head;
		assert(cell != NULL);
		while (cell != NULL &&
			   cell->id != msg->send_agent_to_server.to_player) {
			cell = cell->next;
		}
		assert(cell != NULL);

		SDLNet_TCP_Send(cell->sock, &message, sizeof(message));
		break;
	}

	default:
		SDL_LogWarn(0, "Received a message of wrong type %d\n", msg->type);
		break;
	}
}
