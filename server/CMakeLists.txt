add_executable(server)

target_sources(server
	PRIVATE
		hashmap/hashmap.c
		shared/debug.c
		server/client_list.c
		server/room_list.c
		server/room.c
		server/server.c
		server/main.c)

target_compile_options(server PRIVATE -Wall -Wextra -Wpedantic)

target_link_libraries(server
	SDL2::Main
	SDL2::Net)
