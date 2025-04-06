cmake_minimum_required(VERSION 3.22)

project("BomberMan"
	VERSION 0.1
	DESCRIPTION "Remake of the bomberman game"
	LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

FetchContent_Declare(
	libjuice
	GIT_REPOSITORY https://github.com/paullouisageneau/libjuice.git
	GIT_TAG v1.3.3
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(libjuice)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_net REQUIRED)

add_executable(bomberman)

target_include_directories(bomberman
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${SDL2_INCLUDE_DIRS}
	${SDL2_IMAGE_INCLUDE_DIRS}
	${SDL2_TTF_INCLUDE_DIRS}
	${libjuice_SOURCE_DIR}/include)

target_sources(bomberman
	PRIVATE
		main.c
		graphic.c
		player.c
		bomb.c
		game.c
		collision.c
		ice_network.c
		message.c
		player_list.c
		bomb_list.c
		int_set.c
		menus/main_menu.c
		menus/create_room_menu.c
		menus/join_room_menu.c
		menus/lobby.c
		ui/button.c
		ui/button_list.c
		ui/text_input.c
		ui/gui.c
		cli.c
		client.c
		shared/debug.c
		grid.c
		teleporter.c
		debug.c
  	powerup.c)

target_compile_options(bomberman PRIVATE -Wall -Wextra -Wpedantic)

target_link_libraries(bomberman
	m
	LibJuice::LibJuiceStatic
	SDL2::Main
	SDL2::Image
	SDL2::Net
	SDL2::TTF)

# Matchmaking server

add_executable(server)

target_sources(server
	PRIVATE
		shared/debug.c
		server/client_list.c
		server/room_list.c
		server/room.c
		server/server.c
		server/main.c)

target_include_directories(server
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${SDL2_INCLUDE_DIRS}
	${libjuice_SOURCE_DIR}/include)

target_compile_options(server PRIVATE -Wall -Wextra -Wpedantic)

target_link_libraries(server
	SDL2::Main
	SDL2::Net)
