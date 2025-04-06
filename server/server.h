#ifndef SERVER_H
#define SERVER_H

#include "client_list.h"
#include "shared/client_message.h"
#include "room_list.h"

typedef struct Server {
	ClientList clients;
	IPaddress ip;
	TCPsocket socket;
	RoomList rooms;
} Server;

typedef struct ServerOptions {
	int max_sockets;
	int max_rooms;
} ServerOptions;

int Server_Init(Server* server, const ServerOptions options);

void Server_Finish(Server* server);

void Server_HandleMessage(Server* server,
						  const ClientMessage* msg,
						  TCPsocket source);

#endif
