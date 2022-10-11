#ifndef DRAW_H
#define DRAW_H

#include "rules.h"
#include "state.h"

#include <assert.h>
#include <raylib.h>
#include <stdint.h>

typedef struct {
	uint8_t cellWidth;
	uint8_t cellHeight;
	uint8_t padding;
	uint8_t fontSize;
} Geometry;

static_assert(sizeof(Geometry) == 4, "Geometry size is 4 bytes");

typedef uint16_t          DrawCellMask;
static const DrawCellMask DrawFill    = 1 << 0;
static const DrawCellMask DrawOutline = 1 << 1;
static const DrawCellMask DrawGlow    = 1 << 2;
static const DrawCellMask DrawTrail   = 1 << 3;

void     drawActiveCells(const State *s, const Geometry *g, Color fg, DrawCellMask m);
void     drawBackground(const State *s, const Geometry *g, Color padding, Color bg);
uint16_t drawElement(const Geometry *g, uint16_t offset, Color fg, const char *fmt, ...);
void     drawCursor(Position p, const Geometry *g, Color fg);

#endif
