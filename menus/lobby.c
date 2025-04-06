#include "lobby.h"
#include "game.h"
#include "ice_network.h"
#include "menus/create_room_menu.h"
#include "message.h"
#include "ui/gui.h"

static void on_click_start_game(void* data1, void* _data2) {
	(void)_data2;
	Game* game = data1;

	broadcast_message(&(Message){.type = MESSAGE_TYPE_GAME_START,
								 game->local_player->id,
								 .game_start.time = get_time()});

	lobby_destroy(game->lobby);
	game->status = GAME_STATUS_PLAYING;
	game_spawn_players(game);
}

Lobby* lobby_create(Game* game) {
	Lobby* lobby = malloc(sizeof(*lobby));

	int window_width, window_height;
	SDL_GetRendererOutputSize(game->renderer, &window_width, &window_height);

	SDL_Rect start_button_rect = {
		.x = window_width / 2 - 200,
		.y = window_height - 400,
		.w = 400,
		.h = 100,
	};
	lobby->start_button = gui_create_button(start_button_rect,
											on_click_start_game,
											(void*)game,
											"Start game",
											game->font);

	return lobby;
}

void lobby_destroy(Lobby* lobby) {
	gui_destroy_button(lobby->start_button);
	free(lobby);
}
