#ifndef GAME_H
#define GAME_H

typedef struct Game Game;

#include <SDL.h>
#include <SDL_ttf.h>

#include "bomb.h"
#include "bomb_list.h"
#include "grid.h"
#include "menus/create_room_menu.h"
#include "menus/join_room_menu.h"
#include "menus/lobby.h"
#include "menus/main_menu.h"
#include "network.h"
#include "player.h"
#include "player_list.h"
#include "teleporter.h"

#define second (Uint64)1000000
#define IP_STRING_LENGTH 32

typedef int RoomId;

enum GameStatus {
	GAME_STATUS_MAIN_MENU,
	GAME_STATUS_IN_LOBBY,
	GAME_STATUS_PLAYING,
	GAME_STATUS_MATCH_RESULTS,
	GAME_STATUS_BROWSING_ROOMS,
	GAME_STATUS_CREATE_ROOM,
	GAME_STATUS_JOINING,
};

typedef struct Addr {
	unsigned short port;
	char ip[IP_STRING_LENGTH];
} Addr;

struct Game {
	enum GameStatus status;
	bool running;

	Addr host_addr;

	SDL_Renderer* renderer;
	SDL_Window* window;
	TTF_Font* font;

	PlayerArray players;
	Player* local_player;

	BombList* bombs;

	TeleporterList* teleporters;

	Grid* grid;

	MainMenu* main_menu;
	CreateRoomMenu* create_room_menu;
	JoinRoomMenu* join_room_menu;
	Lobby* lobby;

	Uint64 next_round_start_time;

	Uint32 event_receive_message;

	uint seed;

	RoomId room_id;
};

void game_init(Game* game);
void game_update(Game* game);
void game_handle_events(Game* game);
void game_draw(Game* game);

Bomb* game_add_bomb(Game* game,
					float x,
					float y,
					PlayerId player,
					Uint64 blast_time,
					Uint64 blast_end,
					int range);

void bomb_update(Game* game);

bool check_kill(Game* game, Player* player);

Bomb* place_bomb(Game* game, Player* player);

void game_start_new_round(Game* game);

void game_spawn_players(Game* game);

Uint64 get_time(void);

#endif
