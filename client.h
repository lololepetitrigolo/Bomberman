#ifndef CLIENT_H
#define CLIENT_H

#include <SDL_net.h>

#include "game.h"
#include "shared/server_message.h"

void client_update(Game* game);

void client_handle_message(const ServerMessage* msg, Game* game);

void client_init_connection(const char* from_ip,
							Uint16 from_port,
							const char* server_ip,
							Uint16 server_port);

void client_close_connection(void);

void create_room(const char* room_name, const char* player_name, int seed);

void get_rooms(void);

void join_room(const char* room_name, const char* player_name);

void client_quit(void);

#endif
