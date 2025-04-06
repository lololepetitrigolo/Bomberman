#include "client.h"
#include "debug.h"
#include "game.h"
#include "graphic.h"
#include "ice_network.h"
#include "menus/create_room_menu.h"
#include "menus/join_room_menu.h"
#include "menus/lobby.h"
#include "message.h"
#include "player.h"
#include "player_list.h"
#include "shared/client_message.h"
#include "shared/debug.h"
#include "shared/server_message.h"
#include "ui/gui.h"

#include <SDL_net.h>
#include <SDL_timer.h>
#include <stdio.h>
#include <stdlib.h>

enum ClientStatus {
	CLIENT_STATUS_OFFLINE,
	CLIENT_STATUS_CONNECTING_TO_SERVER,
	CLIENT_STATUS_READY,
	CLIENT_STATUS_CREATING_ROOM,
	CLIENT_STATUS_GETTING_ROOMS,
	CLIENT_STATUS_IN_LOBBY,
	CLIENT_STATUS_JOINING_ROOM,
};

// State of the client.
struct Client {
	bool active;
	TCPsocket socket;
	SDLNet_SocketSet socketset;
	IPaddress address;
	IPaddress server_address;
	enum ClientStatus status;
};

static struct Client client = {
	.active = false,
	.status = CLIENT_STATUS_OFFLINE,
};

// Size of the buffer for incoming server messages.
const int BUFSIZE = 8192;

// The default maximum player count when creating a room.
const uint DEFAULT_MAX_PLAYERS_IN_ROOM = 4;

// Delay before retrying to open a connection with the server (in milliseconds).
const uint CLIENT_RETRY_CONNECT_DELAY = 5000;

// Display the client status on the screen
static void client_display_debug_infos(void) {
	switch (client.status) {
	case CLIENT_STATUS_OFFLINE:
		screen_printf("offline");
		break;
	case CLIENT_STATUS_CONNECTING_TO_SERVER:
		screen_printf("connecting to server...");
		break;
	default:
		screen_printf("online");
		break;
	}
}

void client_update(Game* game) {
	client_display_debug_infos();

	if (!client.active) {
		return;
	}

	char buf[BUFSIZE];
	while (SDLNet_CheckSockets(client.socketset, 0) > 0) {
		SDLNet_TCP_Recv(client.socket, buf, BUFSIZE);
		client_handle_message((const ServerMessage*)buf, game);
	}
}

static void
client_handle_create_room_response(const CreateRoomResponse* response,
								   Game* game) {
	switch (response->status) {

	// Rejections
	case CREATE_ROOM_RESPONSE_TOO_MUCH_ROOMS:
		SDL_LogError(0,
					 "Cannot create a room: there are already too much rooms");
		return;
	case CREATE_ROOM_RESPONSE_NAME_ALREADY_USED:
		SDL_LogError(0, "Cannot create a room: this name is already used");
		return;

	// Success
	case CREATE_ROOM_RESPONSE_OK:
		SDL_LogInfo(0, "Successfully created room");

		game->room_id = response->room_id;
		game->local_player = player_array_insert(&game->players, 0);
		player_init(
			game->local_player,
			0,
			create_player_texture(game->renderer, "triangle_modified.png"));
		player_set_name(game->local_player,
						game->create_room_menu->player_name_input->text);

		create_room_menu_destroy(game->create_room_menu);
		game->lobby = lobby_create(game);
		game->status = GAME_STATUS_IN_LOBBY;
		gui_set_focused_text_input(NULL);

		break;
	}
}

static void client_handle_join_room_response(const JoinRoomResponse* res,
											 Game* game) {
	switch (res->status) {

	// We are allowed to join
	case JOIN_ROOM_RESPONSE_OK: {

		SDL_Log("Joined room [%s] with ID %d", res->room_name, res->player_id);

		// Use the seed provided by the server
		game->seed = res->game_seed;
		game->room_id = res->room_id;
		srand(game->seed);

		SDL_Texture* texture =
			create_player_texture(game->renderer, "triangle_modified.png");

		// Create the local player
		game->local_player =
			player_array_insert(&game->players, res->player_id);
		player_init(game->local_player, res->player_id, texture);
		player_set_name(game->local_player,
						game->join_room_menu->nickname_input->text);

		// Add the other players
		for (int i = 0; i < res->nb_player; i++) {
			Player* player =
				player_array_insert(&game->players, res->players_id[i]);
			player_init(player, res->players_id[i], texture);
			player_set_name(player, res->players_name[i]);

			// init ice connection
			SDL_Log("Sending my agent to player %d", player->id);
			ClientMessage* message =
				(ClientMessage*)malloc(sizeof(ClientMessage));
			message->type = SEND_AGENT_TO_SERVER;
			message->send_agent_to_server.from_player = game->local_player->id;
			message->send_agent_to_server.to_player = player->id;
			strcpy(message->send_agent_to_server.ice_sdp,
				   new_connection(player->id));
			message->send_agent_to_server.room_id = game->room_id;
			SDLNet_TCP_Send(client.socket, message, sizeof(ClientMessage));
		}

		game->status = GAME_STATUS_IN_LOBBY;
		join_room_menu_destroy(game->join_room_menu);
		game->lobby = lobby_create(game);

		client.status = CLIENT_STATUS_IN_LOBBY;

		break;
	}

	// The request was rejected by the server
	case JOIN_ROOM_RESPONSE_INVALID_ROOM:
		SDL_Log(
			"Cannot join room [%s]: %s\n", res->room_name, "invalid room name");
		break;
	case JOIN_ROOM_RESPONSE_NAME_ALREADY_USED:
		SDL_Log("Cannot join room [%s]: %s\n",
				res->room_name,
				"this player name is already used");
		break;
	case JOIN_ROOM_RESPONSE_ROOM_IS_FULL:
		SDL_Log(
			"Cannot join room [%s]: %s\n", res->room_name, "the room is full");
		break;
	}
}

void client_handle_message(const ServerMessage* msg, Game* game) {
	switch (msg->type) {

	case SERVER_HELLO:
		if (client.status == CLIENT_STATUS_CONNECTING_TO_SERVER) {
			SDL_LogInfo(0, "Connected to server");
			client.status = CLIENT_STATUS_READY;
		} else {
			SDL_LogError(0, "Received unexpected hello from server");
		}
		break;

	case SEND_ROOMS:
		SDL_Log("Received the list of rooms: there are %d rooms",
				msg->send_rooms.room_count);

		if (game->status == GAME_STATUS_BROWSING_ROOMS) {
			join_room_menu_update_rooms(
				game, msg->send_rooms.room_count, msg->send_rooms.rooms);
		}

		client.status = CLIENT_STATUS_READY;
		break;

	// The server responds to our request of creating a room.
	case CREATE_ROOM_RESPONSE:
		client_handle_create_room_response(&msg->create_room_response, game);
		client.status = CLIENT_STATUS_READY;
		break;

	// The server responded to our room joining request.
	case JOIN_ROOM_RESPONSE:
		client_handle_join_room_response(&msg->join_room_response, game);
		break;

	// A player joined the room, we must add him to our list of players.
	case ADD_PLAYER_IN_ROOM: {
		SDL_Texture* texture =
			create_player_texture(game->renderer, "triangle_modified.png");

		Player* player = player_array_insert(&game->players,
											 msg->add_player_in_room.player_id);
		player_init(player, msg->join_room_response.player_id, texture);
		player_set_name(player, msg->add_player_in_room.player_name);
		break;
	}

	case GET_AGENT: { // Don't use it we ot longer need it
		SDL_Log("Sending my agent to an other player");
		ClientMessage* message = (ClientMessage*)malloc(sizeof(ClientMessage));
		message->type = SEND_AGENT_TO_SERVER;
		message->send_agent_to_server.from_player = game->local_player->id;
		message->send_agent_to_server.to_player = msg->get_agent.player_id;
		strcpy(message->send_agent_to_server.ice_sdp,
			   new_connection(msg->get_agent.player_id));
		message->send_agent_to_server.room_id = game->room_id;
		break;
	}

	case SEND_AGENT_TO_PLAYER:
		printf("Received this sdp :\n%s\n", msg->send_agent_to_player.ice_sdp);
		SDL_Log("Receive the agent of an other player");

		if (!exist_in_use_index_list(msg->send_agent_to_player.from_player)) {
			SDL_Log("Sending my agent to an other player in response to him");
			ClientMessage* message =
				(ClientMessage*)malloc(sizeof(ClientMessage));
			message->type = SEND_AGENT_TO_SERVER;
			message->send_agent_to_server.from_player = game->local_player->id;
			message->send_agent_to_server.room_id = game->room_id;
			message->send_agent_to_server.to_player =
				msg->send_agent_to_player.from_player;
			strcpy(message->send_agent_to_server.ice_sdp,
				   new_connection(msg->get_agent.player_id));
			SDLNet_TCP_Send(client.socket, message, sizeof(ClientMessage));
		}
		add_remote_agent(msg->send_agent_to_player.from_player,
						 msg->send_agent_to_player.ice_sdp);

		break;

	default:
		SDL_LogError(0, "Client received a bad message !!!\n");
	}
}

/*
 * Tries to connect to the previously provided server.
 * Returns true on success.
 */
static bool client_try_connect(void) {
	char ip_str[64];
	sprint_IPaddress(ip_str, client.server_address);
	SDL_Log("Trying to connect to server at %s...", ip_str);

	client.socket = SDLNet_TCP_Open(&client.server_address);

	if (client.socket) {
		client.status = CLIENT_STATUS_READY;
		client.active = true;
		client.socketset = SDLNet_AllocSocketSet(1);
		SDLNet_AddSocket(client.socketset, (SDLNet_GenericSocket)client.socket);
		SDL_LogInfo(0, "Connected to the server");
		return true;
	} else {
		SDL_LogWarn(0, "Cannot connect to server at %s", ip_str);
		return false;
	}
}

void client_init_connection(const char* from_ip,
							Uint16 from_port,
							const char* server_ip,
							Uint16 server_port) {
	IPaddress my_addr;
	my_addr.port = from_port;

	// Gather our own addresses
	IPaddress my_addresses[8];
	int addr_count = SDLNet_GetLocalAddresses(my_addresses, 8);

	SDL_Log("Available addresses (%d):\n", addr_count);
	for (int i = 0; i < addr_count; i++) {
		bool isdefault = (i == 0);
		char ip_str[64];
		sprint_IPaddress(ip_str, my_addresses[i]);
		SDL_Log("- %s%s\n", ip_str, isdefault ? " (default)" : "");
	}

	if (from_ip) {
		if (SDLNet_ResolveHost(&my_addr, from_ip, from_port) == -1) {
			SDL_LogError(0, "Invalid own address provided\n");
			exit(1);
		}
	} else {
		// If our own address is not given as an argument,
		// choose one by default.
		my_addr = my_addresses[0];
		SDL_Log("No address was given for the client, using default\n");
	}

	char ip_str[64];
	sprint_IPaddress(ip_str, my_addr);
	SDL_Log("Using IP address %s and port %d", ip_str, from_port);

	// Resolve the server host
	IPaddress sv_addr;
	if (SDLNet_ResolveHost(&sv_addr, server_ip, server_port) == -1) {
		SDL_LogCritical(
			0, "Cannot resolve the server hostname \"%s\".\n", server_ip);
		exit(1);
	}

	client.address = my_addr;
	client.server_address = sv_addr;

	client_try_connect();
}

void client_close_connection(void) {
	if (client.socket) {
		SDLNet_TCP_Close(client.socket);
		SDLNet_FreeSocketSet(client.socketset);
	}
	client.active = false;
	client.status = CLIENT_STATUS_OFFLINE;
}

void create_room(const char* room_name, const char* player_name, int seed) {
	if (!client.active && !client_try_connect()) {
		SDL_LogWarn(0, "Cannot send a create room request (not connected)");
		return;
	}

	if (client.status == CLIENT_STATUS_CREATING_ROOM) {
		SDL_LogWarn(0,
					"Tried to send a create room request, but we are already "
					"in the middle of creating a room");
		return;
	}
	ClientMessage msg = {
		.type = CREATE_ROOM,
		.create_room.seed = seed,
		.create_room.max_players = DEFAULT_MAX_PLAYERS_IN_ROOM,
	};
	strcpy(msg.create_room.room_name, room_name);
	strcpy(msg.create_room.player_name, player_name);
	SDL_Log("Asking server to create a room...");
	const int msg_size = sizeof(msg.type) + sizeof(msg.create_room);
	SDLNet_TCP_Send(client.socket, &msg, msg_size);
	client.status = CLIENT_STATUS_CREATING_ROOM;
}

void get_rooms(void) {
	if (!client.active && !client_try_connect()) {
		SDL_LogWarn(0, "Cannot send a get rooms request (not connected)");
		return;
	}

	if (client.status == CLIENT_STATUS_GETTING_ROOMS) {
		return;
		SDL_LogWarn(0,
					"Tried to send a get rooms request, but we are already "
					"waiting for a get rooms response");
	}
	ClientMessage msg = {
		.type = GET_ROOMS,
	};
	SDL_Log("Asking server to get the available rooms...");
	SDLNet_TCP_Send(client.socket, &msg, sizeof(msg.type));
	client.status = CLIENT_STATUS_GETTING_ROOMS;
}

void join_room(const char* room_name, const char* player_name) {
	if (!client.active && !client_try_connect()) {
		SDL_LogWarn(0, "Cannot send a join room request (not connected)");
		return;
	}

	if (client.status == CLIENT_STATUS_JOINING_ROOM) {
		SDL_LogWarn(0,
					"Tried to send a join room request, but we are already "
					"in the middle of joining a room");
		return;
	}
	ClientMessage msg = {.type = JOIN_ROOM};
	strncpy(msg.join_room.room_name, room_name, ROOM_NAME_LENGTH);
	strncpy(msg.join_room.player_name, player_name, PLAYER_NAME_LENGTH);
	SDL_Log("Asking server to join room [%s]", room_name);
	const int msg_size = sizeof(msg.type) + sizeof(msg.join_room);
	SDLNet_TCP_Send(client.socket, &msg, msg_size);
	client.status = CLIENT_STATUS_JOINING_ROOM;
}

void client_quit(void) {
	if (client.active) {
		ClientMessage goodbye = {.type = CLIENT_GOOBYE};
		SDLNet_TCP_Send(client.socket, &goodbye, sizeof(goodbye.type));
		SDLNet_TCP_Close(client.socket);
		SDLNet_FreeSocketSet(client.socketset);
	}
}
