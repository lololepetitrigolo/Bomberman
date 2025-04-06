#ifndef CLI_H
#define CLI_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>

typedef struct Args {
	bool error;

	char* sv_addr;
	Uint16 sv_port;

	char* own_addr;
	Uint16 own_port;
} Args;

void print_usage(char* program_name);

Args parse_args(int argc, char** argv);

#endif

