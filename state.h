#ifndef STATE_H
#define STATE_H

#include "aux.h"
#include "cell.h"
#include "rules.h"

#include <stddef.h>

typedef struct {
	Cell **field;
	int    x;
	int    y;
} State;

Cell   getCell(State *s, Position p);
State *newState(int x, int y);
void   freeState(State *s);
void   activateCell(State *s, Position p);
void   disableCell(State *s, Position p);
void   updateState(State *s, Rule r);
void   permutateState(State *s, int permutations);
void   randomizeState(State *s);
int    countCells(State *s);
void   clearState(State *s);
int    neighbors(State *s, int x, int y);

#endif
