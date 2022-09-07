#ifndef STATE_H
#define STATE_H

#include "aux.h"
#include "cell.h"
#include "rules.h"

#include <stddef.h>

typedef struct {
	Cell **field;
	size_t x, y;
} State;

Cell    getCell(const State *s, Position p);
State  *newState(size_t x, size_t y);
void    freeState(State *s);
void    activateCell(State *s, Position p);
void    disableCell(State *s, Position p);
void    updateState(State *s, Rule r);
void    permutateState(State *s, size_t permutations);
void    randomizeState(State *s);
size_t  countCells(const State *s);
void    clearState(State *s);
uint8_t neighbors(const State *s, Position p);

#endif
