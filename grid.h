#ifndef GRID_H
#define GRID_H

#include <stdbool.h>

typedef enum {
	CELL_TYPE_BREAKABLE,
	CELL_TYPE_UNBREAKABLE,
	CELL_TYPE_BOMB,
	CELL_TYPE_EMPTY,
	CELL_TYPE_KILLING,
    CELL_TYPE_TELEPORTER,
    CELL_TYPE_POWERUP,
} CellType;

typedef struct {
	int i;
	int j;
} CellCoord;

// add a structure for cell tab

// grid stucture
typedef struct {
	CellType** board;
	int height;
	int width;
	int cell_size;
} Grid;

// create a grid
Grid* grid_create(int height, int weight, int cell_size);

// free the Grid grid
void grid_destroy(Grid* grid);

void grid_make_standard(Grid* grid);

//add breakables blocs in the standart grid 
void grid_add_breakable_randomly(Grid* grid, int proportion);

// delete the wall at position coord
// precondition : the wall at coord is breakable
void grid_delete_wall(Grid* grid, CellCoord coord);

// print the grid in a beautiful way
void grid_print(Grid* grid);

bool grid_cell_is_good(CellType cell);

#endif
