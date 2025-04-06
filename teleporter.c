#include "teleporter.h"
#include <assert.h>
#include <stdlib.h>

TeleporterList* create_TeleporterList(int nb_val) {
	TeleporterList* teleporter_list =
		(TeleporterList*)malloc(sizeof(TeleporterList));
	teleporter_list->len = 0;
	teleporter_list->cap = nb_val;
	teleporter_list->data = (Teleporter*)malloc(nb_val * sizeof(Teleporter));
	return teleporter_list;
}

void free_TeleporterList(TeleporterList* list) {
	free(list->data);
	free(list);
}

void TeleporterList_add(Game* game,
						int teleporteur1_i,
						int teleporteur1_j,
						int teleporteur2_i,
						int teleporteur2_j) {
	TeleporterList* list = game->teleporters;
	assert(list->len != list->cap);
	list->data[list->len].teleporteur1_i = teleporteur1_i;
	list->data[list->len].teleporteur1_j = teleporteur1_j;
	list->data[list->len].teleporteur2_i = teleporteur2_i;
	list->data[list->len].teleporteur2_j = teleporteur2_j;
	game->grid->board[teleporteur1_i][teleporteur1_j] = CELL_TYPE_TELEPORTER;
	game->grid->board[teleporteur2_i][teleporteur2_j] = CELL_TYPE_TELEPORTER;
	list->len++;
}

void check_teleporter(Game* game, Player* player) {
	int i = (int)player->y / game->grid->cell_size;
	int j = (int)player->x / game->grid->cell_size;
	Uint64 time = get_time();

	if (player->teleporter_time < 1) {
		player->teleporter_time = time;
	}

	if (game->grid->board[i][j] == CELL_TYPE_TELEPORTER) {
		if (player->teleporter_time + (Uint64)1000000 < time) {
			// find other teleporte0r
			for (int k = 0; k < game->teleporters->len; k++) {
				Teleporter teleporter = game->teleporters->data[k];
				if (teleporter.teleporteur1_i == i &&
					teleporter.teleporteur1_j == j) {
					player->x =
						teleporter.teleporteur2_j * game->grid->cell_size +
						game->grid->cell_size / 2;
					player->y =
						teleporter.teleporteur2_i * game->grid->cell_size +
						game->grid->cell_size / 2;
					player->teleporter_time = 0;
				} else if (teleporter.teleporteur2_i == i &&
						   teleporter.teleporteur2_j == j) {
					player->x =
						teleporter.teleporteur1_j * game->grid->cell_size +
						game->grid->cell_size / 2;
					player->y =
						teleporter.teleporteur1_i * game->grid->cell_size +
						game->grid->cell_size / 2;
					player->teleporter_time = 0;
				}
			}
		}
	} else {
		player->teleporter_time = 0;
	}
}
