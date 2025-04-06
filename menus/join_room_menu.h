#ifndef JOIN_ROOM_MENU_H
#define JOIN_ROOM_MENU_H

typedef struct JoinRoomMenu JoinRoomMenu;

#include <SDL.h>

#include "game.h"
#include "shared/common.h"
#include "ui/button.h"
#include "ui/button_list.h"
#include "ui/text_input.h"

struct JoinRoomMenu {
	Button* home_btn;
	Button* refresh_btn;
	TextInput* nickname_input;
	ButtonList* rooms_button;
	RoomInfo* rooms;
	uint rooms_len;
};

JoinRoomMenu* join_room_menu_create(Game* game);

void join_room_menu_destroy(JoinRoomMenu* menu);

void join_room_menu_update_rooms(Game* game,
								 uint room_count,
								 const RoomInfo* room_infos);

void join_room_menu_draw(Game* game);

#endif
