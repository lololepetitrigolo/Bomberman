#include "join_room_menu.h"
#include "client.h"
#include "ui/button.h"
#include "ui/button_list.h"
#include "ui/gui.h"

#include <SDL.h>
#include <assert.h>
#include <stdlib.h>

#define MAX_NB_ROOM 32

static void on_click_home(void* data, void* _data2) {
	(void)_data2;
	Game* game = data;
	join_room_menu_destroy(game->join_room_menu);
	game->status = GAME_STATUS_MAIN_MENU;
	game->main_menu = main_menu_create(game);
}

static void on_click_join(void* data1, void* data2) {
	Game* game = data1;
	char* room_name = data2;
	join_room(room_name, game->join_room_menu->nickname_input->text);
}

static void on_click_refresh(void* _data, void* _data2) {
	(void)_data;
	(void)_data2;
	get_rooms();
}

static void make_random_name(char* random_name, size_t size) {
	char charset[] = "azertyuiopmlkjhgfdsqwxcvbn";

	for (size_t i = 0; i < size - 1; i++) {
		int key = rand() % (int)(sizeof charset - 1);
		random_name[i] = charset[key];
	}
	random_name[size - 1] = '\0';
}

JoinRoomMenu* join_room_menu_create(Game* game) {
	JoinRoomMenu* menu = malloc(sizeof(*menu));
	menu->rooms = NULL;
	menu->rooms_len = 0;

	int window_width;
	int window_height;
	SDL_GetRendererOutputSize(game->renderer, &window_width, &window_height);

	// Button to go back to the main menu
	SDL_Rect return_to_home_rect = {
		.x = 0,
		.y = 0,
		.w = 150,
		.h = 50,
	};
	menu->home_btn = gui_create_button(return_to_home_rect,
									   &on_click_home,
									   (void*)game,
									   "Return to home",
									   game->font);

	// Button to refresh the list of rooms
	SDL_Rect refresh_btn_rect = {
		.x = window_width - 100,
		.y = 0,
		.w = 100,
		.h = 50,
	};
	menu->refresh_btn = gui_create_button(
		refresh_btn_rect, &on_click_refresh, NULL, "Refresh", game->font);

	// Input to choose our player name
	char nickname[11];
	make_random_name(nickname, sizeof nickname);
	menu->nickname_input =
		gui_create_text_input(nickname,
							  (SDL_Rect){window_width / 2 - 100, 100, 200, 80},
							  (SDL_Color){0, 0, 0, 0},
							  (SDL_Color){255, 255, 255, 255},
							  game->font,
							  20);

	menu->rooms_button = button_list_create(MAX_NB_ROOM);

	return menu;
}

void join_room_menu_destroy(JoinRoomMenu* menu) {
	gui_destroy_button(menu->home_btn);
	gui_destroy_button(menu->refresh_btn);
	gui_destroy_text_input(menu->nickname_input);
	for (uint i = 0; i < menu->rooms_button->len; i++) {
		gui_destroy_button(&menu->rooms_button->buttons[i]);
	}
	button_list_destroy(menu->rooms_button);
	free(menu->rooms);
	free(menu);
}

void join_room_menu_update_rooms(Game* game,
								 uint room_count,
								 const RoomInfo* room_infos) {
	JoinRoomMenu* menu = game->join_room_menu;

	menu->rooms_len = room_count;
	menu->rooms = malloc(room_count * sizeof(*menu->rooms));
	for (uint i = 0; i < room_count; i++) {
		strcpy(menu->rooms[i].name, room_infos[i].name);
	}

	// Destroy the previous buttons
	for (uint i = 0; i < menu->rooms_button->len; i++) {
		gui_destroy_button(&menu->rooms_button->buttons[i]);
	}

	int window_width;
	int window_height;
	SDL_GetRendererOutputSize(game->renderer, &window_width, &window_height);

	SDL_Rect dest = {
		.x = window_width / 4,
		.y = window_height / 4,
		.w = window_width / 2,
		.h = 30,
	};

	for (uint i = 0; i < room_count; i++) {
		Button* button = gui_create_button(dest,
										   on_click_join,
										   (void*)game,
										   game->join_room_menu->rooms[i].name,
										   game->font);
		button->on_click.data2 = game->join_room_menu->rooms[i].name;

		button_list_add_button(game->join_room_menu->rooms_button, button);
		dest.y += dest.h + 5;
	}
}

void join_room_menu_draw(Game* game) {}