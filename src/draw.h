#ifndef DRAW_H
#define DRAW_H

#include "rules.h"
#include "state.h"

#include <raylib.h>
#include <stdint.h>

typedef struct {
	Color trailColor;
	bool  drawTrail;
} TrailOpts;

typedef struct {
	Color cellColor;
	Color outlineColor;
	bool  drawOutline;
} CellOpts;

typedef struct {
	Color backgroundColor;
	Color paddingColor;
	Color cursorColor;
} InterfaceOpts;

typedef struct {
	uint8_t cellWidth;
	uint8_t cellHeight;
	uint8_t padding;
} GeometryOpts;

typedef struct {
	bool       paused;
	uint16_t   targetFPS;
	bool       drawFPS;
	size_t     numberOfCells;
	bool       countCells;
	Variation *variation;
} Status;

void drawActiveCells(const State *s, const GeometryOpts *g, const CellOpts *c);
void drawBackground(const State *s, const GeometryOpts *g, const InterfaceOpts *i);
void drawStatus(const Status *s, const GeometryOpts *g, const CellOpts *c, const TrailOpts *t);
void drawCursor(Position p, const InterfaceOpts *i, const GeometryOpts *g);

#endif
