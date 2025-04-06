#include "create_room_menu.h"
#include "client.h"
#include "game.h"
#include "ui/gui.h"

static void on_click_home(void* data, void* _data2) {
	(void)_data2;
	Game* game = data;
	create_room_menu_destroy(game->create_room_menu);
	game->status = GAME_STATUS_MAIN_MENU;
	game->main_menu = main_menu_create(game);
}

static void on_click_create_room(void* data, void* _data2) {
	(void)_data2;
	Game* game = data;
	create_room(game->create_room_menu->room_name_input->text,
				game->create_room_menu->player_name_input->text,
				42);
}

CreateRoomMenu* create_room_menu_create(Game* game) {
	CreateRoomMenu* menu = malloc(sizeof(*menu));

	int window_width;
	int window_height;
	SDL_GetRendererOutputSize(game->renderer, &window_width, &window_height);

	SDL_Color text_color = {0x00, 0x00, 0x00, 0xFF};
	SDL_Color background_color = {0xFF, 0xFF, 0xFF, 0xFF};

	// Text input for the room name
	SDL_Rect room_name_position = {
		window_width / 4,
		window_height / 4,
		window_width / 2,
		40,
	};
	menu->room_name_input = gui_create_text_input(
		"", room_name_position, background_color, text_color, game->font, 18);
	menu->room_name_input->on_return.fn = on_click_create_room;
	menu->room_name_input->on_return.data1 = game;

	// Text input for the player's name
	// TODO: initialize it with the last player name used
	SDL_Rect player_name_pos = {
		window_width / 4,
		window_height / 4 + 60,
		window_width / 2,
		40,
	};
	menu->player_name_input = gui_create_text_input(
		"Kevin", player_name_pos, background_color, text_color, game->font, 18);

	// Button to return to the main menu
	SDL_Rect return_to_home_rect = {
		0,
		0,
		150,
		50,
	};
	menu->home_btn = gui_create_button(return_to_home_rect,
									   &on_click_home,
									   (void*)game,
									   "Return to home",
									   game->font);

	// Button to create the room
	SDL_Rect create_room_rect = {
		window_width / 2 - 100,
		window_height / 2,
		200,
		50,
	};
	menu->create_btn = gui_create_button(create_room_rect,
										 &on_click_create_room,
										 (void*)game,
										 "Create room",
										 game->font);

	return menu;
}

void create_room_menu_destroy(CreateRoomMenu* menu) {
	gui_destroy_button(menu->create_btn);
	gui_destroy_button(menu->home_btn);
	gui_destroy_text_input(menu->room_name_input);
	gui_destroy_text_input(menu->player_name_input);
	free(menu);
}

void create_room_menu_draw(CreateRoomMenu* menu, SDL_Renderer* renderer) {}
