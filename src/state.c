#include "state.h"

#include "aux.h"

#include <assert.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static bool
validPosition(const State *s, Position p) {
	return p.x < s->x && p.y < s->y;
}

static uint8_t
neighbors(const State *s, Position p) {
	uint8_t c = 0;
	for (size_t i = 0; i < 3; i++) {
		for (size_t j = 0; j < 3; j++) {
			if (i == 1 && j == 1)
				continue;
			const Position q = {.x = p.x + i - 1, .y = p.y + j - 1};
			if (validPosition(s, q) && isActive(getCell(s, q)))
				c++;
		}
	}
	return c;
}

Cell
getCell(const State *s, Position p) {
	return s->field[p.x][p.y];
}

State *
newState(size_t x, size_t y) {
	State *s = malloc(sizeof(State));
	assert(s);
	s->field = malloc(x * sizeof(Cell *));
	assert(s->field);
	s->x = x;
	s->y = y;

	for (size_t i = 0; i < s->x; i++) {
		s->field[i] = malloc(y * sizeof(Cell));
		assert(s->field[i]);
		for (size_t j = 0; j < s->y; j++)
			s->field[i][j] = inactiveCell();
	}
	return s;
}

static void
copyState(State *dst, const State *src) {
	for (size_t x = 0; x < src->x; x++)
		for (size_t y = 0; y < src->y; y++)
			dst->field[x][y] = src->field[x][y];
}

void
freeState(State *s) {
	for (size_t x = 0; x < s->x; x++)
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
	for (size_t x = 0; x < s->x; x++) {
		for (size_t y = 0; y < s->y; y++) {
			const Position p = {.x = x, .y = y};
			if (shouldLive(r, neighbors(s, p), isActive(getCell(s, p))))
				activateCell(next, p);
		}
	}
	copyState(s, next);
	freeState(next);
}

void
permutateState(State *s, size_t permutations) {
	for (size_t i = 0; i < permutations; i++) {
		const Position p = {
			.x = GetRandomValue(0, s->x - 1),
			.y = GetRandomValue(0, s->y - 1),
		};
		activateCell(s, p);
	}
}

void
randomizeState(State *s) {
	for (size_t x = 0; x < s->x; x++) {
		for (size_t y = 0; y < s->y; y++) {
			const Position pos = {.x = x, .y = y};
			randBool() ? activateCell(s, pos) : disableCell(s, pos);
		}
	}
}

size_t
countCells(const State *s) {
	size_t c = 0;
	for (size_t x = 0; x < s->x; x++)
		for (size_t y = 0; y < s->y; y++)
			if (isActive(getCell(s, (Position){.x = x, .y = y})))
				c++;
	return c;
}

void
clearState(State *s) {
	for (size_t x = 0; x < s->x; x++)
		for (size_t y = 0; y < s->y; y++)
			disableCell(s, (Position){.x = x, .y = y});
}
