#ifndef DRAW_H
#define DRAW_H

#include "rules.h"
#include "state.h"

#include <raylib.h>
#include <stdint.h>

typedef struct {
	Color   cellColor;
	Color   outlineColor;
	Color   trailColor;
	Color   backgroundColor;
	Color   paddingColor;
	Color   cursorColor;
	bool    drawOutline;
	bool    drawTrail;
	uint8_t cellWidth;
	uint8_t cellHeight;
	uint8_t padding;
} DrawOptions;

typedef struct {
	bool     paused;
	uint16_t targetFPS;
	bool     drawFPS;
	size_t   numberOfCells;
	bool     countCells;
	RuleSet *ruleSet;
} Status;

void drawCursor(Position p, const DrawOptions *o);
void drawGame(const State *s, const Status *t, const DrawOptions *o);

#endif
