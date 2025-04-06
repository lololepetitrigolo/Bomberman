#ifndef MESSAGE_H
#define MESSAGE_H

// parse the actions of the player to send the string to other players
#include "game.h"
#include "network.h"
#include "player.h"
#include "powerup.h"

#define USERNAME_MAX_LENGTH 32

typedef enum MessageType {
	/* The game is starting */
	MESSAGE_TYPE_GAME_START,
	/* A player is moving */
	MESSAGE_TYPE_UPDATE_PLAYER,
	/* A bomb is placed*/
	MESSAGE_TYPE_UPDATE_BOMB,
	/* A powerup is discover or consume */
	MESSAGE_TYPE_UPDATE_POWERUP,
	/* Tells others that we disconnect */
	MESSAGE_TYPE_DISCONNECT,
} MessageType;

typedef struct MessageGameStart {
	Uint64 time;
} MessageGameStart;

typedef struct MessageUpdatePlayer {
	float player_x;
	float player_y;
	float orientation;
	Uint64 time;
} MessageUpdatePlayer;

typedef struct MessageUpdateBomb {
	float bomb_x;
	float bomb_y;
	Uint64 time;
} MessageUpdateBomb;

// If no powerup is taken powerup_j_i = -1
typedef struct MessageUpdatePowerup {
	bool is_taken; // true if the powerup must disapear, false otherwise
	int powerup_i;
	int powerup_j;
	Powerup powerup_type;
	Uint64 time;
} MessageUpdatePowerup;

typedef struct MessageDisconnect {
	PlayerId id;
} MessageDisconnect;

typedef struct MessageAck {
	Uint64 timestamp;
} MessageAck;

typedef struct Message {
	MessageType type;
	PlayerId player_id;
	union {
		MessageGameStart game_start;

		MessageUpdatePlayer update_player;
		MessageUpdateBomb update_bomb;
		MessageUpdatePowerup update_powerup;

		MessageDisconnect disconnect;

		MessageAck ack;
	};
} Message;

typedef struct MessageAndAddr {
	Addr addr;
	Message message;
} MessageAndAddr;

// Create a message to update the position of the player
// and maybe place a bomb.
Message* create_message_update(Player* player, Bomb* bomb);

// Create a message containing a connection request.
Message* create_message_connect(Player* player);

// Print a message on stdout.
void print_message(Message* msg);

// Send a message to a player
void send_message(PlayerId target, Message* message);

// Send a message to the given address and port
// ssize_t send_message_to_addr(Addr addr, Message* message);

// Do what must be done with a received message
void handle_message(Message* msg, Game* game, Addr* sender);

// Send a message to all other players.
void broadcast_message(Message* message);
#endif
