#include "draw.h"

#include "aux.h"

#include <raylib.h>
#include <stdio.h>

static Rectangle
cellRectangle(Position p, const DrawOptions *o) {
	return (Rectangle){
		.x      = o->padding + o->cellWidth * p.x,
		.y      = o->padding + o->cellHeight * p.y,
		.width  = o->cellWidth,
		.height = o->cellHeight,
	};
}

static Rectangle
fieldRectangle(const State *s, const DrawOptions *o) {
	return (Rectangle){
		.x      = o->padding,
		.y      = o->padding,
		.width  = s->x * o->cellWidth,
		.height = s->y * o->cellHeight,
	};
}

static void
drawCell(Position p, const DrawOptions *o) {
	DrawRectangleRec(cellRectangle(p, o), o->cellColor);
}

static void
drawOutline(Position p, const DrawOptions *o) {
	DrawRectangleLinesEx(cellRectangle(p, o), 1, o->outlineColor);
}

static void
drawState(const State *s, const DrawOptions *o) {
	for (size_t x = 0; x < s->x; x++) {
		for (size_t y = 0; y < s->y; y++) {
			const Position p = {.x = x, .y = y};
			if (isActive(getCell(s, p))) {
				drawCell(p, o);
				if (o->drawOutline)
					drawOutline(p, o);
			}
		}
	}
}

static void
drawBackground(const State *s, const DrawOptions *o) {
	ClearBackground(o->paddingColor);
	DrawRectangleRec(fieldRectangle(s, o), o->backgroundColor);
}

static void
drawStatus(const Status *s, const DrawOptions *o) {
	const char   *prefix   = "<<<";
	const char   *suffix   = ">>>";
	const uint8_t fontSize = o->padding;
	const uint8_t spacing  = o->padding;
	uint16_t      offset   = o->padding;

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
	if (o->drawOutline) {
		const char *outline = "OUTLINE";
		DrawText(outline, offset, 0, fontSize, RAYWHITE);
		offset += spacing + MeasureText(outline, fontSize);
	}
	if (o->drawTrail) {
		const char *trail = "TRAIL";
		DrawText(trail, offset, 0, fontSize, o->trailColor);
		offset += spacing + MeasureText(trail, fontSize);
	}
	if (s->countCells) {
		const size_t nCellsLen = 32;
		char         nCells[nCellsLen];
		snprintf(nCells, nCellsLen, "CELLS: %zu", s->numberOfCells);
		DrawText(nCells, offset, 0, fontSize, o->cellColor);
		offset += spacing + MeasureText(nCells, fontSize);
	}
	DrawText(suffix, offset, 0, fontSize, GRAY);
}

void
drawCursor(Position p, const DrawOptions *o) {
	DrawRectangleLinesEx(cellRectangle(p, o), 1, o->cursorColor);
}

void
drawGame(const State *s, const Status *t, const DrawOptions *o) {
	drawBackground(s, o);
	drawStatus(t, o);
	drawState(s, o);
}
