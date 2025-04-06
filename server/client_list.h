#ifndef SERVER_CLIENT_LIST_H
#define SERVER_CLIENT_LIST_H

#include <SDL_net.h>

typedef struct Client {
	char name[32];
	TCPsocket socket;
	SDLNet_SocketSet socketset;
} Client;

typedef struct ClientList {
	size_t cap;
	size_t len;
	Client* data;
} ClientList;

void ClientList_Init(ClientList* list, size_t capacity);

void ClientList_Finish(ClientList* list);

void ClientList_Push(ClientList* list, Client client);

void ClientList_RemoveBySocket(ClientList* list, TCPsocket socket);

#endif
