#ifndef TELEPORTER_H
#define TELEPORTER_H

typedef struct{
    int teleporteur1_i;
    int teleporteur1_j;
    int teleporteur2_i;
    int teleporteur2_j;
} Teleporter;

typedef struct{
    Teleporter* data;
    int cap;
    int len;
} TeleporterList;

#include "game.h"
#include "player.h"

TeleporterList* create_TeleporterList(int nb_val);

void free_TeleporterList(TeleporterList* list);

void TeleporterList_add(Game*game, int teleporteur1_i, int teleporteur1_j, int teleporteur2_i, int teleporteur2_j);

void check_teleporter(Game*game, Player*player);

#endif
