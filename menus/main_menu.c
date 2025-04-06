#include "main_menu.h"
#include "client.h"
#include "game.h"
#include "menus/create_room_menu.h"
#include "menus/lobby.h"
#include "player_list.h"
#include "ui/gui.h"

#include <SDL_image.h>

static void on_click_local_game(void* data1, void* _data2) {
	(void)_data2;
	Game* game = data1;
	main_menu_destroy(game->main_menu);
	game->status = GAME_STATUS_IN_LOBBY;
	game->lobby = lobby_create(game);
	game->local_player = player_array_insert(&game->players, 0);
	SDL_Texture* tex = IMG_LoadTexture(game->renderer, "triangle_modified.png");
	player_init(game->local_player, 0, tex);
}

static void on_click_join(void* data, void* _data2) {
	Game* game = data;
	(void)_data2;
	main_menu_destroy(game->main_menu);
	game->status = GAME_STATUS_BROWSING_ROOMS;
	game->join_room_menu = join_room_menu_create(game);
	get_rooms();
}

static void on_click_create(void* data, void* _data2) {
	Game* game = data;
	(void)_data2;
	main_menu_destroy(game->main_menu);
	game->status = GAME_STATUS_CREATE_ROOM;
	game->create_room_menu = create_room_menu_create(game);
}

MainMenu* main_menu_create(Game* game) {
	int window_width;
	int window_height;
	SDL_GetRendererOutputSize(game->renderer, &window_width, &window_height);

	MainMenu* menu = malloc(sizeof(*menu));

	const int button_height = 50;
	const int button_width = window_width / 4;
	const int button_spacing = 10;

	SDL_Rect btn_rect = {
		.x = window_width / 2 - button_width / 2,
		.y = window_height / 2 - (button_height + button_spacing) * 3,
		.w = button_width,
		.h = button_height,
	};

	menu->local_game_btn = gui_create_button(
		btn_rect, &on_click_local_game, (void*)game, "Local game", game->font);

	btn_rect.y += button_height + button_spacing;
	menu->create_room_btn = gui_create_button(
		btn_rect, &on_click_create, (void*)game, "Create a room", game->font);

	btn_rect.y += button_height + button_spacing;
	menu->join_room_btn = gui_create_button(
		btn_rect, &on_click_join, (void*)game, "Join a room", game->font);

	return menu;
}

void main_menu_destroy(MainMenu* menu) {
	gui_destroy_button(menu->local_game_btn);
	gui_destroy_button(menu->create_room_btn);
	gui_destroy_button(menu->join_room_btn);
	free(menu);
}

void main_menu_update_size(MainMenu* menu, SDL_Renderer* renderer) {
	int window_width;
	int window_height;
	SDL_GetRendererOutputSize(renderer, &window_width, &window_height);

	const int button_height = 50;
	const int button_width = window_width / 4;
	const int button_spacing = 10;

	SDL_Rect btn_rect = {
		.x = window_width / 2 - button_width / 2,
		.y = window_height / 2 - (button_height + button_spacing) * 3,
		.w = button_width,
		.h = button_height,
	};

	menu->local_game_btn->rect = btn_rect;

	btn_rect.y += button_height + button_spacing;
	menu->create_room_btn->rect = btn_rect;

	btn_rect.y += button_height + button_spacing;
	menu->join_room_btn->rect = btn_rect;
}

void main_menu_draw(MainMenu* menu, SDL_Renderer* renderer) {}
