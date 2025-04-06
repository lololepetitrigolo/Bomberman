#include "bomb_list.h"

BombList* BombList_create(int nb_val) {
	BombList* list = (BombList*)malloc(sizeof(BombList));
	list->cap = nb_val;
	list->len = 0;
	list->data = (Bomb*)malloc(nb_val * sizeof(Bomb));
	return list;
}

Bomb* BombList_add(BombList* list,
				   float x,
				   float y,
				   PlayerId player,
				   Uint64 blast_time,
				   Uint64 blast_end,
				   int range) {
	if (list->cap == list->len) {
		list->cap *= 2;
		list->data = (Bomb*)realloc(list->data, list->cap * sizeof(Bomb));
	}
	list->data[list->len] = (Bomb){x, y, player, blast_time, blast_end, range};
	list->len += 1;
	return &list->data[list->len - 1];
}

void BombList_remove(BombList* list, int i) {
	if (list->len == 1) {
		list->len = 0;
	} else {
		list->data[i] = list->data[list->len - 1];
		list->len -= 1;
	}
}

void BombList_free(BombList* list) {
	free(list->data);
	free(list);
}
