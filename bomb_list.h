#ifndef BOMB_LIST_H
#define BOMB_LIST_H

typedef struct BombList BombList;
#include "bomb.h"

// Dynamic Array for Bomb
struct BombList {
	Bomb* data;
	Uint32 cap;
	Uint32 len;
};

// Alloc memory for a Bomb an add this Bomb in list
Bomb* BombList_add(BombList* list,
				   float x,
				   float y,
				   PlayerId player,
				   Uint64 blast_time,
				   Uint64 blast_end,
				   int range);

void BombList_remove(BombList* list, int i);

BombList* BombList_create(int nb_val);

void BombList_free(BombList* list);

#endif
