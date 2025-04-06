#include <SDL.h>
#include <SDL_net.h>
#include <assert.h>
#include <stdbool.h>

#include "server.h"
#include "shared/debug.h"
#include "shared/server_message.h"

#define BUFSIZE 8192

int main(int argc, char** argv) {
if (SDL_Init(0) == -1) {
		printf("SDL_Init: %s\n", SDL_GetError());
		exit(1);
	}

	if (SDLNet_Init() == -1) {
		printf("SDLNet_Init: %s\n", SDL_GetError());
		exit(1);
	}

	const ServerOptions options = {
		.max_sockets = 16,
		.max_rooms = 4,
	};

	Server server;
	if (Server_Init(&server, options) == -1) {
		exit(1);
	}

	printf("Listening on port %d\n", server.ip.port);

	char buf[BUFSIZE];

	bool running = true;
	while (running) {

		TCPsocket new_connection = SDLNet_TCP_Accept(server.socket);
		if (new_connection) {
			IPaddress* ip = SDLNet_TCP_GetPeerAddress(new_connection);
			printf("New connection from ");
			print_IPaddress(*ip);
			printf("\n");
			Client client = {.socket = new_connection,
							 .socketset = SDLNet_AllocSocketSet(1)};
			SDLNet_AddSocket(client.socketset,
							 (SDLNet_GenericSocket)client.socket);
			ClientList_Push(&server.clients, client);
			ServerMessage hello = {.type = SERVER_HELLO};
			SDLNet_TCP_Send(new_connection, &hello, sizeof(hello));
		}

		for (size_t i = 0; i < server.clients.len; i++) {
			if (SDLNet_CheckSockets(server.clients.data[i].socketset, 0)) {
				SDLNet_TCP_Recv(server.clients.data[i].socket, buf, BUFSIZE);
				ClientMessage* message = (ClientMessage*)buf;
				Server_HandleMessage(
					&server, message, server.clients.data[i].socket);
			}
		}

		SDL_Delay(10);
	}

	Server_Finish(&server);

	SDLNet_Quit();
	SDL_Quit();

	return EXIT_SUCCESS;
}
