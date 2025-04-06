/*#ifndef NETWORK_H
#define NETWORK_H

#include <SDL.h>

#include "player.h"

#define IP_STRING_LENGTH 32

typedef struct Addr {
	unsigned short port;
	char ip[IP_STRING_LENGTH];
} Addr;

// Initialize a socket on the given IP addr and port.
// This function should be called only once.
// Returns -1 if en error occured.
// The memory must be freed by calling `network_finish()`.
int network_init(char* ip, Uint16 port);

// Free the memory used by the networking module.
void network_finish(void);

// Add a peer with a given ID.
int network_add_player(PlayerId id, char* ip_addr, unsigned short port);

// Remove the peer with a given ID.
void network_remove_player(PlayerId id);

Addr network_get_player_addr(PlayerId id);

// Send a special data structure to the player with the given ID.
// Returns -1 if an error happened.
ssize_t send_data(PlayerId target, unsigned char* data, int sizeof_data);

ssize_t send_data_to_addr(Addr addr, unsigned char* data, int sizeof_data);

// Send data to all peers.
// Returns -1 if an error happened.
ssize_t broadcast_data(unsigned char* data, int sizeof_data);

// Listen for incoming messages and push events accordingly.
// Loops until an error happens or the local player disconnects.
// Returns EXIT_SUCCESS if an error happened and EXIT_SUCCESS otherwise.
int recv_data(Uint32 event_type_receive_message);

#endif*/
