#include "cli.h"

void print_usage(char* program_name) {
	printf("Usage: %s <YOUR_IP> <YOUR_PORT>\n", program_name);
	printf("Options:\n");
	printf("  -s, --server <IP> <PORT>\tUse the server at given address\n");
	exit(1);
}

Args parse_args(int argc, char** argv) {
	Args args;
	args.error = false;
	args.sv_addr = "localhost";
	args.sv_port = 8888;
	args.own_addr = NULL;
	args.own_port = 12345;

	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "--server.ip")) {
			if (i + 1 >= argc) {
				printf("Missing arguments to --server.ip / -s.ip\n");
				args.error = true;
				return args;
			}
			args.sv_addr = argv[i + 1];
			i++;
			continue;
		}

		if (!strcmp(argv[i], "--server.port")) {
			if (i + 1 >= argc) {
				printf("Missing arguments to --server.port / -s.port\n");
				args.error = true;
				return args;
			}
			args.sv_port = atoi(argv[i + 1]);
			i++;
			continue;
		}

		if (!strcmp(argv[i], "--my.ip")) {
			if (i + 1 >= argc) {
				printf("Wrong arguments to --my.ip\n");
				args.error = true;
				return args;
			}
			args.own_addr = argv[i + 1];
			i++;
			continue;
		}

		if (!strcmp(argv[i], "--my.port")) {
			if (i + 1 >= argc) {
				printf("Wrong arguments to --my.port\n");
				args.error = true;
				return args;
			}
			args.own_port = atoi(argv[i + 1]);
			i++;
			continue;
		}
	}
	
	return args;
}
