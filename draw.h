#ifndef DRAW_H
#define DRAW_H

#include "rules.h"
#include "state.h"

#include <raylib.h>

typedef struct {
	Color color;
	Color outlineColor;
	bool  drawOutline;
} DrawOptions;

typedef struct {
	bool     paused;
	int      targetFPS;
	bool     drawFPS;
	bool     drawOutline;
	bool     drawTrail;
	int      numberOfCells;
	bool     countCells;
	RuleSet *ruleSet;
} Status;

void drawState(State *s, DrawOptions o);
void drawBackground();
void drawStatus(Status s);
void drawCursor(Position p);
void drawCell(Position p, Color c);
void drawOutline(Position p, Color c);

#endif
