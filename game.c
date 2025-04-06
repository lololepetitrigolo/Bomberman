#include <SDL_net.h>
#include <assert.h>
#include <errno.h>
#include <sys/time.h>

#include "bomb_list.h"
#include "client.h"
#include "debug.h"
#include "game.h"
#include "graphic.h"
#include "grid.h"
#include "ice_network.h"
#include "menus/create_room_menu.h"
#include "menus/join_room_menu.h"
#include "menus/main_menu.h"
#include "message.h"
#include "player.h"
#include "ui/gui.h"

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 600;

const int TIME_BETWEEN_ROUNDS = 3000 /* ms */;

static void debug_print_players(Game* game) {
	screen_printf("Players:");
	for (uint id = 0; id < game->players.len; id++) {
		if (game->players.data[id].exists) {
			screen_printf(" - %d: %s%s",
						  id,
						  game->players.data[id].player.name,
						  (PlayerId)id == game->local_player->id ? " (you)"
																 : "");
		}
	}
}

Uint64 get_time(void) {
	struct timeval current_time;
	gettimeofday(&current_time, NULL);
	Uint64 time = current_time.tv_sec * second + current_time.tv_usec;
	return time;
}

Bomb* game_add_bomb(Game* game,
					float x,
					float y,
					PlayerId player,
					Uint64 blast_time,
					Uint64 blast_end,
					int range) {
	int j = x / game->grid->cell_size;
	int i = y / game->grid->cell_size;

	game->grid->board[i][j] = CELL_TYPE_BOMB;
	return BombList_add(
		game->bombs, x, y, player, blast_time, blast_end, range);
}

void remove_bomb(Game* game, int indice) {
	if ((Uint32)indice >= game->bombs->len) {
		printf("Indice out of range");
	} else {
		BombList_remove(game->bombs, indice);
	}
}

void bomb_update(Game* game) {
	// remove the bombs that have definitly finish to explode
	// kill the players that are in range of bombs explosion
	// Maybe think about explosion animation
	// delete the bomb from BombsList and it animation when the time
	// bomb.blast_end has come supprime la bombe ainsi que son animation
	// quand elle a passé le temps de vie + le temps d'explo
	Uint64 time = get_time();
	// to change : put the values in function of player possibilites
	for (int i = 0; (uint32_t)i < game->bombs->len; i++) {
		Bomb bomb = game->bombs->data[(int)i];
		bool is_exploding = time >= bomb.blast_time;
		bool finished_explode = time >= bomb.blast_end;
		if (is_exploding) {
			if (finished_explode) {
				killings_cells(game, &bomb, false);
				remove_bomb(game, i);
			} else {
				killings_cells(game, &bomb, true);
			}
		}
	}
}

bool check_kill(Game* game, Player* player) {
	int i_p = (int)floor(player->y / game->grid->cell_size);
	int j_p = (int)floor(player->x / game->grid->cell_size);

	return game->grid->board[i_p][j_p] == CELL_TYPE_KILLING;
}

Bomb* place_bomb(Game* game, Player* player) {
	Uint64 time = get_time();
	int j = player->x / game->grid->cell_size;
	int i = player->y / game->grid->cell_size;
	if (time - player->time_last_bomb > 10 &&
		game->grid->board[i][j] == CELL_TYPE_EMPTY) {
		Bomb* bomb = game_add_bomb(game,
								   player->x,
								   player->y,
								   player->id,
								   time + 1000000,
								   time + 1500000,
								   2 + player->range);
		player->time_last_bomb = time;
		return bomb;
	}
	return NULL;
}

/*
 * If the game is finished, returns the winner, otherwise returns NULL.
 */
Player* game_check_victory(const Game* game) {
	// Count alive players
	int alive_player_count = 0;
	Player* maybe_winner = NULL;
	for (uint i = 0; i < game->players.len; i++) {
		if (game->players.data[i].exists &&
			game->players.data[i].player.is_alive) {
			if (!maybe_winner) {
				maybe_winner = &game->players.data[i].player;
			}
			++alive_player_count;
		}
	}

	// Check if the game is finished
	if (alive_player_count <= 1 && game->players.len > 1) {
		return maybe_winner;
	} else {
		return NULL;
	}
}

void game_spawn_players(Game* game) {
	CellCoord spawn_positions[4] = {
		{1, 1},
		{game->grid->width - 2, 1},
		{1, game->grid->height - 2},
		{game->grid->width - 2, game->grid->height - 2},
	};
	for (uint i = 0; i < game->players.len; i++) {
		CellCoord pos = spawn_positions[i];
		game->players.data[i].player.x = (pos.i + 0.5) * game->grid->cell_size;
		game->players.data[i].player.y = (pos.j + 0.5) * game->grid->cell_size;
	}
}

void game_start_new_round(Game* game) {
	// Revive all players
	for (uint i = 0; i < game->players.len; i++) {
		if (game->players.data[i].exists) {
			game->players.data[i].player.is_alive = true;
		}
	}
	for (uint i = 0; i < game->bombs->len; i++) {
		killings_cells(game, &game->bombs->data[i], false);
	}
	game->bombs->len = 0;
	grid_make_standard(game->grid);
	grid_add_breakable_randomly(game->grid, 100);
	game_spawn_players(game);
}

void game_init(Game* game) {
	game->running = true;
	game->local_player = NULL;
	game->bombs = BombList_create(100);

	// Create an empty grid
	game->grid = grid_create(19, 19, WINDOW_HEIGHT / 21);
	// Transform the grid
	grid_make_standard(game->grid);
	grid_add_breakable_randomly(game->grid, 100);

	// Create teleporters
	game->teleporters = create_TeleporterList(1);
	TeleporterList_add(game, 5, 13, 15, 6);

	// Place a few powerups
	game->grid->board[3][10] = (CellType)POWERUP_BOMB_RANGE;
	game->grid->board[10][5] = (CellType)POWERUP_SPEED;

	player_array_init(&game->players, 4);

	// Create the window
	game->window = SDL_CreateWindow("BomberMan",
									SDL_WINDOWPOS_UNDEFINED,
									SDL_WINDOWPOS_UNDEFINED,
									WINDOW_WIDTH,
									WINDOW_HEIGHT,
									SDL_WINDOW_RESIZABLE);

	// Create the renderer
	game->renderer = SDL_CreateRenderer(
		game->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	// Setup font
	game->font = TTF_OpenFont("assets/Hack-Regular.ttf", 16);

	// Register an event for messages received from the network
	game->event_receive_message = SDL_RegisterEvents(1);
	if (game->event_receive_message == ((Uint32)-1)) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Cannot register network events");
		exit(1);
	}
	init_event_type(game->event_receive_message);

	game->join_room_menu = NULL;
	game->create_room_menu = NULL;
	game->lobby = NULL;

	game->status = GAME_STATUS_MAIN_MENU;
	game->main_menu = main_menu_create(game);
}

void game_handle_events(Game* game) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			if (game->local_player) {
				broadcast_message(&(Message){
					.type = MESSAGE_TYPE_DISCONNECT,
					.disconnect.id = game->local_player->id,
				});
			}
			game->running = false;
			break;
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
				if (game->status == GAME_STATUS_MAIN_MENU) {
					main_menu_update_size(game->main_menu, game->renderer);
				}

				if (game->status != GAME_STATUS_PLAYING)
					break;

				int window_height;
				int window_width;
				SDL_GetRendererOutputSize(
					game->renderer, &window_width, &window_height);
				int new_cell_size = window_height / (game->grid->height + 2);
				game->local_player->x = game->local_player->x /
										game->grid->cell_size * new_cell_size;
				game->local_player->y = game->local_player->y /
										game->grid->cell_size * new_cell_size;
				game->local_player->speed = game->local_player->speed /
											game->grid->cell_size *
											new_cell_size;
				game->grid->cell_size = new_cell_size;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			gui_handle_button_up_event(event.button);
			break;
		case SDL_TEXTINPUT:
			gui_handle_text_input_event(event.text);
			break;
		case SDL_KEYDOWN:
			gui_handle_keyboard_event(event.key);
			break;
		default:
			// A custom event
			if (event.type >= SDL_USEREVENT) {
				// We received a message
				if (event.type == game->event_receive_message) {
					Message* message = event.user.data1;
					// TODO: remove param `sender`
					handle_message(message, game, NULL);
					// free(msg);
				}
				break;
			}
		}
	}
}

void game_update(Game* game) {
	Uint64 time = get_time();

	switch (game->status) {
	case GAME_STATUS_PLAYING: {
		const Uint8* keyboard = SDL_GetKeyboardState(NULL);
		float move_x = 0;
		float move_y = 0;

		if (game->local_player->is_alive) {
			move_y -= (keyboard[SDL_SCANCODE_W] || keyboard[SDL_SCANCODE_UP]);
			move_x -= (keyboard[SDL_SCANCODE_A] || keyboard[SDL_SCANCODE_LEFT]);
			move_y += (keyboard[SDL_SCANCODE_S] || keyboard[SDL_SCANCODE_DOWN]);
			move_x +=
				(keyboard[SDL_SCANCODE_D] || keyboard[SDL_SCANCODE_RIGHT]);

			if (move_x != 0 || move_y != 0) {
				player_move(game->local_player, game->grid, move_x, move_y);
			}

			if (keyboard[SDL_SCANCODE_SPACE]) {
				Bomb* bomb = place_bomb(game, game->local_player);
				if (bomb != NULL) {
					broadcast_message(&(Message){
						.type = MESSAGE_TYPE_UPDATE_BOMB,
						.player_id = game->local_player->id,
						.update_bomb.time = bomb->blast_time - 1000000,
						.update_bomb.bomb_x = bomb->x / game->grid->cell_size,
						.update_bomb.bomb_y = bomb->y / game->grid->cell_size,
					});
				}
			}
		}

		// Check for kills
		for (uint id = 0; id < game->players.len; id++) {
			if (game->players.data[id].exists &&
				game->players.data[id].player.is_alive) {
				if (check_kill(game, &game->players.data[id].player))
					game->players.data[id].player.is_alive = false;
			}
		}

		// Check powerup
		Powerup powerup_type = check_powerup(game->grid, game->local_player);
		if (powerup_type >= CELL_TYPE_POWERUP) {
			broadcast_message(&(Message){
				.type = MESSAGE_TYPE_UPDATE_POWERUP,
				.player_id = game->local_player->id,
				.update_powerup.is_taken = true,
				.update_powerup.powerup_type = powerup_type,
				.update_powerup.powerup_i =
					(int)game->local_player->y / game->grid->cell_size,
				.update_powerup.powerup_j =
					(int)game->local_player->x / game->grid->cell_size,
			});
		}

		for (int i = 0; i < (int)game->players.len; i++)
			check_powerup_end(&game->players.data[i].player);

		// Check teleporter
		check_teleporter(game, game->local_player);

		// Send a message indicating our new position
		broadcast_message(&(Message){
			.type = MESSAGE_TYPE_UPDATE_PLAYER,
			.player_id = game->local_player->id,
			.update_player.time = time,
			.update_player.player_x =
				game->local_player->x / game->grid->cell_size,
			.update_player.player_y =
				game->local_player->y / game->grid->cell_size,
			.update_player.orientation = player_orient(move_x, move_y)});

		bomb_update(game);

		// Debug informations
		debug_print_players(game);
		screen_printf("Position: (%.2f, %.2f)",
					  game->local_player->x,
					  game->local_player->y);
		screen_printf("Powerups:");
		for (int i = 0; i < game->local_player->nb_powerup; i++) {
			screen_printf(
				" - %s (%.1fs)",
				get_powerup_name(game->local_player->powerups[i].type),
				(game->local_player->powerups[i].end_time - time) / 1000000.);
		}

		// Check if the round is finished
		Player* winner;
		if ((winner = game_check_victory(game))) {
			game->next_round_start_time = SDL_GetTicks() + TIME_BETWEEN_ROUNDS;
			SDL_Log("%s won the match!", winner->name);
		}

		break;
	}

	case GAME_STATUS_MATCH_RESULTS:
		if (SDL_GetTicks() > game->next_round_start_time) {
			game->status = GAME_STATUS_PLAYING;
			game_start_new_round(game);
		}
		break;

	case GAME_STATUS_IN_LOBBY:
		debug_print_players(game);
		/* fallthrough */
	case GAME_STATUS_CREATE_ROOM:
	case GAME_STATUS_MAIN_MENU:
	case GAME_STATUS_BROWSING_ROOMS:
	case GAME_STATUS_JOINING:
		client_update(game);
		break;
	}
}

void game_draw(Game* game) {
	switch (game->status) {

	case GAME_STATUS_PLAYING:
		// Draw the grid
		draw_grid(game->renderer, game->grid);

		// Draw the players
		for (uint id = 0; id < game->players.len; id++) {
			if (game->players.data[id].exists &&
				game->players.data[id].player.is_alive) {
				draw_player(
					game->renderer, &game->players.data[id].player, game->grid);
			}
		}
		break;

	case GAME_STATUS_MATCH_RESULTS:
		break;

	case GAME_STATUS_JOINING:
		break;

	case GAME_STATUS_CREATE_ROOM:
		create_room_menu_draw(game->create_room_menu, game->renderer);
		break;

	case GAME_STATUS_MAIN_MENU:
		main_menu_draw(game->main_menu, game->renderer);
		break;

	case GAME_STATUS_BROWSING_ROOMS:
		join_room_menu_draw(game);
		break;

	case GAME_STATUS_IN_LOBBY:
		break;
	}
}
