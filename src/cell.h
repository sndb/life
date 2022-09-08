#ifndef CELL_H
#define CELL_H

#include <stdbool.h>

typedef bool Cell;

Cell inactiveCell();
Cell activeCell();
bool isActive(Cell c);

#endif
