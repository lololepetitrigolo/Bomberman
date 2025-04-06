#include "message.h"
#include "assert.h"
#include "game.h"
#include "graphic.h"
#include "grid.h"
#include "ice_network.h"
#include "menus/lobby.h"
#include "player_list.h"
#include <SDL_log.h>
#include <string.h>
#include <sys/time.h>

// testing (not finished)
unsigned char* return_string(Message* packet) {
	unsigned char* message = (unsigned char*)packet;

	for (size_t i = 0; i < sizeof(Message); ++i) {
		printf("%02X ", message[i]);
	}
	printf("\n");

	return message;
}

void print_message(Message* msg) {
	switch (msg->type) {
	case MESSAGE_TYPE_UPDATE_PLAYER:
		printf("Message { pid: %d, x: %f, y: %f}\n",
			   msg->player_id,
			   msg->update_player.player_x,
			   msg->update_player.player_y);
		break;
	case MESSAGE_TYPE_UPDATE_BOMB:
		printf("Message bomb : { pid:%d, bomb_x: %f, bomb_y: %f\n",
			   msg->player_id,
			   msg->update_bomb.bomb_x,
			   msg->update_bomb.bomb_y);
		break;
	case MESSAGE_TYPE_UPDATE_POWERUP:
		printf("Message powerup {pid: %d, powerup_i: %d, powerup_j: %d, "
			   "powerup_type: %d}\n",
			   msg->player_id,
			   msg->update_powerup.powerup_i,
			   msg->update_powerup.powerup_j,
			   msg->update_powerup.powerup_type);
		break;
	default:
		printf("print_message not implemented for this type of message");
		break;
	}
}

void send_message(PlayerId target, Message* message) {
	send_data(target, (char*)message, sizeof(Message));
}
/*
ssize_t send_message_to_addr(Addr addr, Message* message) {
	return send_data_to_addr(addr, (unsigned char*)message, sizeof(Message));
}
*/
void broadcast_message(Message* message) {
	broadcast_data((char*)message, sizeof(Message));
}

void handle_message(Message* msg, Game* game, Addr* sender) {

	switch (msg->type) {

	case MESSAGE_TYPE_UPDATE_PLAYER: {
		PlayerId id = msg->player_id;
		// Only apply this update's movement if it is more recent
		// than the last update received.
		if (msg->update_player.time >=
			game->players.data[id].player.most_recent_update) {
			game->players.data[id].player.most_recent_update =
				msg->update_player.time;
			game->players.data[id].player.x =
				msg->update_player.player_x * game->grid->cell_size;
			game->players.data[id].player.y =
				msg->update_player.player_y * game->grid->cell_size;
			game->players.data[id].player.orientation =
				msg->update_player.orientation;
		}
		break;
	}

	case MESSAGE_TYPE_UPDATE_BOMB: {
		// If a bomb was placed, add it.
		game_add_bomb(game,
					  msg->update_bomb.bomb_x * game->grid->cell_size,
					  msg->update_bomb.bomb_y * game->grid->cell_size,
					  msg->player_id,
					  msg->update_bomb.time + 1000000,
					  msg->update_bomb.time + 1500000,
					  2 + game->players.data[msg->player_id].player.range);
		break;
	}

	case MESSAGE_TYPE_UPDATE_POWERUP:
		if (msg->update_powerup.is_taken) {
			remove_powerup(game->grid,
						   msg->update_powerup.powerup_i,
						   msg->update_powerup.powerup_j);
			add_powerup_to_player(&game->players.data[msg->player_id].player,
								  msg->update_powerup.powerup_type);
		} else {
			add_powerup(game->grid,
						msg->update_powerup.powerup_i,
						msg->update_powerup.powerup_j,
						msg->update_powerup.powerup_type);
		}
		break;

	case MESSAGE_TYPE_DISCONNECT:
		// TODO: make sure that the sender is the one who disconnects
		player_array_remove(&game->players, msg->disconnect.id);
		// TODO: remove the associated network connection (agent)
		break;
	case MESSAGE_TYPE_GAME_START:
		SDL_Log("Game is starting");
		if (game->status == GAME_STATUS_IN_LOBBY) {
			lobby_destroy(game->lobby);
			game->status = GAME_STATUS_PLAYING;
			game_spawn_players(game);
		}
		break;
	}
}
