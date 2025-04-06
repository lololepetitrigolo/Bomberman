#include "shared/debug.h"

void print_IPaddress(IPaddress addr) {
	Uint32 ipaddr = SDL_SwapBE32(addr.host);
	printf("%d.%d.%d.%d",
		   ipaddr >> 24,
		   (ipaddr >> 16) & 0xff,
		   (ipaddr >> 8) & 0xff,
		   ipaddr & 0xff);
	if (addr.port) {
		printf(":%d", addr.port);
	}
}

void sprint_IPaddress(char* dest, IPaddress addr) {
	Uint32 ipaddr = SDL_SwapBE32(addr.host);
	int len = sprintf(dest,
					  "%d.%d.%d.%d",
					  ipaddr >> 24,
					  (ipaddr >> 16) & 0xff,
					  (ipaddr >> 8) & 0xff,
					  ipaddr & 0xff);
	if (addr.port) {
		sprintf(&dest[len + 1], ":%d", addr.port);
	}
}
