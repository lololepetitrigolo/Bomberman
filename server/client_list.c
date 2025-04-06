#include "client_list.h"

#include <SDL_net.h>
#include <assert.h>

void ClientList_Init(ClientList* list, size_t capacity) {
	assert(capacity > 0);
	list->cap = capacity;
	list->len = 0;
	list->data = malloc(capacity * sizeof(*list->data));
}

void ClientList_Finish(ClientList* list) {
	list->cap = 0;
	list->len = 0;
	free(list->data);
}

void ClientList_Push(ClientList* list, Client client) {
	if (list->len == list->cap) {
		list->cap *= 2;
		list->data = realloc(list->data, list->cap * sizeof(*list->data));
	}

	list->data[list->len] = client;
	list->len++;
}

void ClientList_RemoveBySocket(ClientList* list, TCPsocket socket) {
	for (size_t i = 0; i < list->len; i++) {
		if (list->data[i].socket == socket) {
			SDLNet_FreeSocketSet(list->data[i].socketset);
			SDLNet_TCP_Close(list->data[i].socket);
			list->data[i] = list->data[list->len - 1];
			list->len--;
			return;
		}
	}
}
