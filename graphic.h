#include <SDL.h>

#include "bomb.h"
#include "grid.h"
#include "player.h"

void draw_grid(SDL_Renderer* renderer, Grid* grid);

void draw_player(SDL_Renderer* renderer, Player* player, Grid* grid);

void draw_bomb(SDL_Renderer* renderer, Bomb* bomb);

SDL_Texture* create_player_texture(SDL_Renderer* renderer, char* image_name);
