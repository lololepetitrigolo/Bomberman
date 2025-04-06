#ifndef COLLISION_H
#define COLLISION_H

#include "stdbool.h"
#include <SDL.h>

bool point_in_rect(SDL_FPoint* p, SDL_FRect* a);

bool rect_intersects(SDL_FRect* a, SDL_FRect* b);

#endif
