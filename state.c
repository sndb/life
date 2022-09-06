#include "state.h"

#include "aux.h"

#include <assert.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static bool
validPosition(State *s, Position p) {
	return p.x >= 0 && p.x < s->x && p.y >= 0 && p.y < s->y;
}

Cell
getCell(State *s, Position p) {
	return s->field[p.x][p.y];
}

State *
newState(int x, int y) {
	State *s = malloc(sizeof(State));
	assert(s != NULL);
	s->x     = x;
	s->y     = y;
	s->field = malloc(x * sizeof(Cell *));
	assert(s->field != NULL);

	for (int i = 0; i < s->x; i++) {
		s->field[i] = malloc(y * sizeof(Cell));
		assert(s->field[i] != NULL);
		for (int j = 0; j < s->y; j++)
			s->field[i][j] = inactiveCell();
	}
	return s;
}

static void
copyState(State *dst, State *src) {
	for (int x = 0; x < src->x; x++)
		for (int y = 0; y < src->y; y++)
			dst->field[x][y] = src->field[x][y];
}

void
freeState(State *s) {
	for (int x = 0; x < s->x; x++)
		free(s->field[x]);
	free(s->field);
	free(s);
}

void
activateCell(State *s, Position p) {
	if (validPosition(s, p))
		s->field[p.x][p.y] = activeCell();
}

void
disableCell(State *s, Position p) {
	if (validPosition(s, p))
		s->field[p.x][p.y] = inactiveCell();
}

void
updateState(State *s, Rule r) {
	State *next = newState(s->x, s->y);
	for (int x = 0; x < s->x; x++) {
		for (int y = 0; y < s->y; y++) {
			Position p = {.x = x, .y = y};
			if (shouldLive(r, neighbors(s, x, y), isActive(getCell(s, p))))
				activateCell(next, p);
		}
	}
	copyState(s, next);
	freeState(next);
}

void
permutateState(State *s, int permutations) {
	for (int i = 0; i < permutations; i++) {
		activateCell(s,
		             (Position){
				 .x = GetRandomValue(0, s->x - 1),
				 .y = GetRandomValue(0, s->y - 1),
			     });
	}
}

void
randomizeState(State *s) {
	for (int x = 0; x < s->x; x++)
		for (int y = 0; y < s->y; y++)
			if (randBool())
				activateCell(s, (Position){.x = x, .y = y});
}

int
countCells(State *s) {
	int c = 0;
	for (int x = 0; x < s->x; x++)
		for (int y = 0; y < s->y; y++)
			if (isActive(getCell(s, (Position){.x = x, .y = y})))
				c++;
	return c;
}

void
clearState(State *s) {
	for (int x = 0; x < s->x; x++)
		for (int y = 0; y < s->y; y++)
			disableCell(s, (Position){.x = x, .y = y});
}

int
neighbors(State *s, int x, int y) {
	int c = 0;
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if (i == 0 && j == 0)
				continue;
			const Position p = {.x = x + i, .y = y + j};
			if (validPosition(s, p) && isActive(getCell(s, p)))
				c++;
		}
	}
	return c;
}
