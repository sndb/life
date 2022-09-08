#include "draw.h"

#include "aux.h"
#include "cell.h"

#include <raylib.h>
#include <stdio.h>

static Rectangle
cellRectangle(Position p, const GeometryOpts *g) {
	return (Rectangle){
		.x      = g->padding + g->cellWidth * p.x,
		.y      = g->padding + g->cellHeight * p.y,
		.width  = g->cellWidth,
		.height = g->cellHeight,
	};
}

static Rectangle
fieldRectangle(const State *s, const GeometryOpts *g) {
	return (Rectangle){
		.x      = g->padding,
		.y      = g->padding,
		.width  = s->x * g->cellWidth,
		.height = s->y * g->cellHeight,
	};
}

static void
drawCell(Position p, Color c, const GeometryOpts *g) {
	DrawRectangleRec(cellRectangle(p, g), c);
}

static void
drawOutline(Position p, Color c, const GeometryOpts *g) {
	DrawRectangleLinesEx(cellRectangle(p, g), 1, c);
}

void
drawActiveCells(const State *s, const GeometryOpts *g, const CellOpts *c) {
	for (size_t x = 0; x < s->x; x++) {
		for (size_t y = 0; y < s->y; y++) {
			const Position p = {.x = x, .y = y};
			if (isActive(getCell(s, p))) {
				drawCell(p, c->cellColor, g);
				if (c->drawOutline)
					drawOutline(p, c->outlineColor, g);
			}
		}
	}
}

void
drawBackground(const State *s, const GeometryOpts *g, const InterfaceOpts *i) {
	ClearBackground(i->paddingColor);
	DrawRectangleRec(fieldRectangle(s, g), i->backgroundColor);
}

void
drawStatus(const Status *s, const GeometryOpts *g, const CellOpts *c, const TrailOpts *t) {
	const uint8_t fontSize = g->padding;
	const uint8_t spacing  = g->padding;
	uint16_t      offset   = g->padding;

	const char *prefix = "<<<";
	DrawText(prefix, offset, 0, fontSize, GRAY);
	offset += spacing + MeasureText(prefix, fontSize);

	const char *rule = ruleName(getRuleName(s->ruleSet));
	DrawText(rule, offset, 0, fontSize, WHITE);
	offset += spacing + MeasureText(rule, fontSize);

	if (s->paused) {
		const char *paused = "PAUSED";
		DrawText(paused, offset, 0, fontSize, YELLOW);
		offset += spacing + MeasureText(paused, fontSize);
	}
	if (s->drawFPS) {
		const size_t fpsLen = 32;
		char         fps[fpsLen];
		snprintf(fps, fpsLen, "FPS: %d (%d)", GetFPS(), s->targetFPS);
		DrawText(fps, offset, 0, fontSize, GREEN);
		offset += spacing + MeasureText(fps, fontSize);
	}
	if (c->drawOutline) {
		const char *outline = "OUTLINE";
		DrawText(outline, offset, 0, fontSize, c->outlineColor);
		offset += spacing + MeasureText(outline, fontSize);
	}
	if (t->drawTrail) {
		const char *trail = "TRAIL";
		DrawText(trail, offset, 0, fontSize, t->trailColor);
		offset += spacing + MeasureText(trail, fontSize);
	}
	if (s->countCells) {
		const size_t nCellsLen = 32;
		char         nCells[nCellsLen];
		snprintf(nCells, nCellsLen, "CELLS: %zu", s->numberOfCells);
		DrawText(nCells, offset, 0, fontSize, c->cellColor);
		offset += spacing + MeasureText(nCells, fontSize);
	}

	const char *suffix = ">>>";
	DrawText(suffix, offset, 0, fontSize, GRAY);
}

void
drawCursor(Position p, const InterfaceOpts *i, const GeometryOpts *g) {
	DrawRectangleLinesEx(cellRectangle(p, g), 1, i->cursorColor);
}
