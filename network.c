/*#include <SDL.h>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "message.h"
#include "network.h"
#include "player.h"

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

// strcut to acces to good index in player_list
typedef struct {
	int len;
	int cap;
	int* data;
} UseIndexList;

// dynamic array for player network information
typedef struct {
	Addr* data;
	Uint32 cap;
} SocketList;

static const int INITIAL_SOCKET_LIST_CAPACITY = 10;
static SOCKET sock;

static UseIndexList* use_index_list = NULL;
static SocketList* socket_list = NULL;

// nb_val is the initial length of the array
UseIndexList* UseIndexList_create(int nb_val) {
	UseIndexList* list = (UseIndexList*)malloc(sizeof(UseIndexList));
	list->cap = nb_val;
	list->len = 0;
	list->data = (int*)malloc(nb_val * sizeof(int));
	return list;
}

void UseIndexList_add(UseIndexList* list, int player_id) {
	if (list->cap == list->len) {
		list->data = (int*)realloc(list->data, (2 * list->cap) * sizeof(int));
		list->cap *= 2;
	}
	list->data[list->len] = player_id;
	list->len += 1;
}

static void UseIndexList_remove_index(UseIndexList* list, int i) {
	if (list->len == 1) {
		list->len = 0;
	} else {
		list->data[i] = list->data[list->len - 1];
		list->len -= 1;
	}
}

static void UseIndexList_remove(UseIndexList* list, PlayerId id) {
	for (int i = 0; i < list->len; i++) {
		if (list->data[i] == id) {
			UseIndexList_remove_index(list, i);
			return;
		}
	}
}

void UseIndexList_free(UseIndexList* list) {
	free(list->data);
	free(list);
}

void init_use_index_list(void) {
	use_index_list = UseIndexList_create(INITIAL_SOCKET_LIST_CAPACITY);
}

SocketList* SocketList_create(int capacity) {
	SocketList* list = (SocketList*)malloc(sizeof(SocketList));
	list->cap = capacity;
	list->data = (Addr*)malloc(capacity * sizeof(Addr));
	return list;
}

void SocketList_add(char* ip_addr, Uint16 port, PlayerId player_id) {
	if (player_id == (int)socket_list->cap) {
		socket_list->cap *= 2;
		socket_list->data = realloc(
			socket_list->data, socket_list->cap * sizeof(*socket_list->data));
	}

	strcpy(socket_list->data[player_id].ip, ip_addr);
	socket_list->data[player_id].port = port;

	UseIndexList_add(use_index_list, player_id);
}

void SocketList_remove(PlayerId id) { UseIndexList_remove(use_index_list, id); }

void SocketList_free(void) {
	free(socket_list->data);
	free(socket_list);
}

void init_socket_list(void) {
	socket_list = SocketList_create(INITIAL_SOCKET_LIST_CAPACITY);
}

// initialize the sock of the local player (this is usefull to send data)
int init_socket(char* ip_addr, unsigned short port) {
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = inet_addr(ip_addr);
	int a = bind(sock, (SOCKADDR*)&sin, sizeof(SOCKADDR));
	if (a == -1) {
		printf("init_socket: %s\n", strerror(errno));
	}
	return a;
}

int network_add_player(int player_id, char* ip_addr, unsigned short port) {
	// When there is a new player we save his address
	SocketList_add(ip_addr, port, player_id);
	return 0;
}

void network_remove_player(PlayerId id) { SocketList_remove(id); }

ssize_t send_data(PlayerId id, unsigned char* data, int sizeof_data) {
	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(socket_list->data[id].ip);
	sin.sin_port = htons(socket_list->data[id].port);
	return sendto(
		sock, data, sizeof_data, 0, (struct sockaddr*)&sin, sizeof(SOCKADDR));
}

ssize_t send_data_to_addr(Addr addr, unsigned char* data, int sizeof_data) {
	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(addr.ip);
	sin.sin_port = htons(addr.port);
	return sendto(
		sock, data, sizeof_data, 0, (struct sockaddr*)&sin, sizeof(SOCKADDR));
}

int recv_data(Uint32 event_type_receive_message) {
	// Buffer for received data
	unsigned char data[sizeof(Message)];
	size_t data_length = sizeof(Message);
	bzero(data, data_length);

	// Address of the sender
	SOCKADDR_IN sender_sin;
	socklen_t sender_sin_length;

	while (true) {
		// Listening for incoming data
		ssize_t bytes_count = recvfrom(sock,
									   data,
									   data_length,
									   0,
									   (SOCKADDR*)&sender_sin,
									   &sender_sin_length);
		if (bytes_count == -1) {
			return EXIT_FAILURE;
		}

		assert((size_t)bytes_count == data_length);

		// We push the received message to the SDL event queue
		SDL_Event ev;
		SDL_memset(&ev, 0, sizeof(ev));
		ev.type = event_type_receive_message;
		ev.user.code = 0xbeef;

		Addr sender_addr;
		strcpy(sender_addr.ip, inet_ntoa(sender_sin.sin_addr));
		sender_addr.port = ntohs(sender_sin.sin_port);

		MessageAndAddr* msg_and_addr = malloc(sizeof(MessageAndAddr));
		memcpy(&msg_and_addr->message, data, data_length);
		msg_and_addr->addr = sender_addr;
		ev.user.data1 = msg_and_addr;
		SDL_PushEvent(&ev);
	}
}

ssize_t broadcast_data(unsigned char* data, int sizeof_data) {
	// Temporary implementation, later we can inline this
	ssize_t len = 0;
	for (int i = 0; i < use_index_list->len; i++) {
		len = send_data(use_index_list->data[i], data, sizeof_data);
	}
	return len;
}

int network_init(char* ip, Uint16 port) {
	init_socket_list();
	init_use_index_list();
	return init_socket(ip, port);
}

void network_finish(void) {
	SocketList_free();
	UseIndexList_free(use_index_list);
}

Addr network_get_player_addr(PlayerId id) {
	assert(0 <= id);
	return socket_list->data[id];
}
*/
