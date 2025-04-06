#ifndef CREATE_ROOM_MENU_H
#define CREATE_ROOM_MENU_H

typedef struct CreateRoomMenu CreateRoomMenu;

#include "game.h"
#include "ui/button.h"
#include "ui/text_input.h"

/*
 * The menu to create a room, with a text_input for the name of
 * the room.
 */
struct CreateRoomMenu {
	Button* create_btn;
	Button* home_btn;
	TextInput* room_name_input;
	TextInput* player_name_input;
};

CreateRoomMenu* create_room_menu_create(Game* game);

void create_room_menu_destroy(CreateRoomMenu* menu);

void create_room_menu_draw(CreateRoomMenu* menu, SDL_Renderer* renderer);

#endif
