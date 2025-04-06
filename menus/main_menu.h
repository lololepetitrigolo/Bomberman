#ifndef MAIN_MENU_H
#define MAIN_MENU_H

typedef struct MainMenu MainMenu;

#include "game.h"
#include "ui/button.h"

struct MainMenu {
	Button* local_game_btn;
	Button* create_room_btn;
	Button* join_room_btn;
};

MainMenu* main_menu_create(Game* game);

void main_menu_destroy(MainMenu* menu);

void main_menu_draw(MainMenu* menu, SDL_Renderer* renderer);

void main_menu_update_size(MainMenu* menu, SDL_Renderer* renderer);

#endif
