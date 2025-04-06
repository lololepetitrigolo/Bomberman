#include "grid.h"
#include "game.h"
#include "powerup.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Grid* grid_create(int height, int width, int cell_size) {
	Grid* grid = (Grid*)malloc(sizeof(Grid));
	
	grid->board = (CellType**)malloc(sizeof(CellType*) * height);
	for (int i = 0; i < height; i++) {
		grid->board[i] = (CellType*)calloc(width, sizeof(CellType));
		for (int j = 0; j < width; j++) {
			grid->board[i][j] = CELL_TYPE_EMPTY;
		}
	}

	for (int i = 0; i < height; i++) {
		grid->board[i][0] = CELL_TYPE_UNBREAKABLE;
		grid->board[i][width - 1] = CELL_TYPE_UNBREAKABLE;
	}

	for (int j = 0; j < width; j++) {
		grid->board[0][j] = CELL_TYPE_UNBREAKABLE;
		grid->board[height - 1][j] = CELL_TYPE_UNBREAKABLE;
	}

	grid->height = height;
	grid->width = width;
	grid->cell_size = cell_size;
	return grid;
}

void grid_destroy(Grid* grid) {
	for (int i = 0; i < grid->height; i++) {
		free(grid->board[i]);
	}
	free(grid->board);
	free(grid);
}

// grid is a grid with unbreakable along the borders
void grid_make_standard(Grid* grid) {
	for (int i = 2; i < (grid->height - 2); i += 2) {
		for (int j = 2; j < (grid->width - 2); j += 2) {
			grid->board[i][j] = CELL_TYPE_UNBREAKABLE;
		}
	}
}

void grid_add_breakable_randomly(Grid* grid, int proportion) {
	int p = 5;

	for (int i = 1; i < grid->width - 1; i++) {
		for (int j = 1; j < grid->width - 1; j++) {
			if (i <= p)
				if (j < p - i || j > grid->width + i - p - 1)
					continue;

			if (i >= grid->height - p - 1)
				// most boring formula to find :(
				if (j < p - grid->height + i + 1 ||
					j > 2 * grid->height - 2 - i - p)
					continue;

			if (grid->board[i][j] == CELL_TYPE_EMPTY) {
				int random_number = rand() % 100 + 1;
				if (random_number <= proportion) {
					grid->board[i][j] = CELL_TYPE_BREAKABLE;
				}
			}
		}
	}
}

void grid_delete_wall(Grid* grid, CellCoord coord) {
	grid->board[coord.i][coord.j] = CELL_TYPE_EMPTY;
}

void grid_print(Grid* grid) {
	for (int i = 0; i < grid->height; i++) {
		for (int j = 0; j < grid->width; j++) {
			printf(" %d ", grid->board[i][j]);
		}
		printf("\n");
	}

	printf("\n######################\n");
}

bool grid_cell_is_good(CellType cell) {
	return cell == CELL_TYPE_BREAKABLE || cell == CELL_TYPE_KILLING ||
		   cell == CELL_TYPE_EMPTY || cell >= CELL_TYPE_POWERUP;
}
