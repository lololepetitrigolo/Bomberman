#ifndef LOBBY_H
#define LOBBY_H

#include "ui/button.h"

typedef struct Lobby {
	Button* start_button;
	Button* return_home_button;
} Lobby;

#include "game.h"

Lobby* lobby_create(Game* game);
void lobby_destroy(Lobby* menu);

#endif
