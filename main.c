#include <SDL.h>
#include <SDL_events.h>
#include <SDL_image.h>
#include <SDL_log.h>
#include <SDL_mouse.h>
#include <SDL_net.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#include "cli.h"
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
#include "powerup.h"
#include "shared/debug.h"
#include "teleporter.h"
#include "ui/gui.h"

const Uint32 frame_rate = 1000 / 60;

static void log_output(void* _data,
					   int _category,
					   SDL_LogPriority priority,
					   const char* text) {
	(void)_data;
	(void)_category;

	time_t t;
	time(&t);
	struct tm* timeinfo = localtime(&t);
	struct timeval tv;
	gettimeofday(&tv, tzname);

	printf("\033[90m%.2d:%.2d:%.2d.%.6ld\033[0m ",
		   timeinfo->tm_hour,
		   timeinfo->tm_min,
		   timeinfo->tm_sec,
		   tv.tv_usec);

	switch (priority) {
	case SDL_LOG_PRIORITY_INFO:
		printf("\033[32mINFO\033[0m ");
		break;
	case SDL_LOG_PRIORITY_ERROR:
	case SDL_LOG_PRIORITY_CRITICAL:
		printf("\033[30mERROR\033[0m ");
		break;
	case SDL_LOG_PRIORITY_WARN:
		printf("\033[33mWARN\033[0m ");
		break;
	case SDL_LOG_PRIORITY_DEBUG:
	default:
		break;
	}

	printf("%s\n", text);
}

static void init_sdl(void) {
	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO) == -1) {
		SDL_LogError(0, "Cannot initialize SDL: %s", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	// Initialize SDL_image with support for PNG files
	if (IMG_Init(IMG_INIT_PNG) == -1) {
		SDL_LogError(0, "Cannot initialize SDL_image: %s", SDL_GetError());
		exit(1);
	}
	atexit(IMG_Quit);

	// Initialize SDL_ttf to load fonts and draw text.
	if (TTF_Init() == -1) {
		SDL_LogError(0, "Cannot initialize SDL_ttf: %s", SDL_GetError());
		exit(1);
	}
	atexit(TTF_Quit);

	// Initialize SDLNet to contact the matchmaking server
	if (SDLNet_Init() == -1) {
		SDL_LogError(0, "Cannot initialize SDLNet: %s", SDL_GetError());
		exit(1);
	}
	atexit(SDLNet_Quit);

	SDL_LogSetOutputFunction(&log_output, NULL);
}

int main(int argc, char** argv) {
	// intialization of seed
	srand(time(NULL));

	init_sdl();

	// Get the IP addresses and other stuff from command line args
	Args args = parse_args(argc, argv);
	if (args.error) {
		printf("Wrong arguments!\n");
		print_usage(argv[0]);
		exit(1);
	}

	// Initialize UI
	gui_init();
	atexit(gui_quit);

	Game game;
	game_init(&game);
	// TODO: game cleanup

	// Initialize debug drawing
	debug_init();
	atexit(debug_quit);

	// Connect to the server
	client_init_connection(
		args.own_addr, args.own_port, args.sv_addr, args.sv_port);
	atexit(client_quit);

	// Initialize peer to peer networking
	init_agent_list();
	init_sdp_list();
	init_use_index_list();

	Uint32 last_frame = 0;

	// Main loop
	while (game.running) {
		// Read all events (keyboard, mouse, ...)
		game_handle_events(&game);

		// This block will run at fixed time intervals (~60Hz)
		if (last_frame + frame_rate <= SDL_GetTicks()) {
			gui_update();
			game_update(&game);

			// Prepare a new frame
			SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
			SDL_RenderClear(game.renderer);

			game_draw(&game);
			gui_draw(game.renderer);
			debug_draw(game.renderer);

			// Display the frame
			SDL_RenderPresent(game.renderer);

			last_frame = SDL_GetTicks();
		}

		// Let the CPU breath a little
		SDL_Delay(5);
	}

	// Cleanup SDL
	SDL_DestroyRenderer(game.renderer);
	SDL_DestroyWindow(game.window);

	SDL_Log("Goodbye!");

	return EXIT_SUCCESS;
}
