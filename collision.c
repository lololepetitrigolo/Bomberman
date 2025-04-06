#include "collision.h"

bool point_in_rect(SDL_FPoint* p, SDL_FRect* a) {
	return (a->x <= p->x && p->x <= a->x + a->w && a->y <= p->y &&
			p->y <= a->y + a->h);
}

bool rect_intersects(SDL_FRect* a, SDL_FRect* b) {
	SDL_FPoint corners[4] = {{b->x, b->y},
							 {b->x + b->w, b->y},
							 {b->x, b->y + b->h},
							 {b->x + b->w, b->y + b->h}};
	for (int i = 0; i < 4; i++)
		if (point_in_rect(&corners[i], a))
			return true;
	return false;
}
