#include "draw.h"

#include "aux.h"
#include "config.h"
#include "rules.h"
#include "state.h"

#include <raylib.h>
#include <stdio.h>

static Rectangle
cellRectangle(Position p) {
	int xOffset = padding + cellWidth * p.x;
	int yOffset = padding + cellHeight * p.y;
	return (Rectangle){
	    .x      = xOffset,
	    .y      = yOffset,
	    .width  = cellWidth,
	    .height = cellHeight,
	};
}

void
drawState(State *s, DrawOptions o) {
	for (int x = 0; x < s->x; x++) {
		for (int y = 0; y < s->y; y++) {
			if (isActive(getCell(s, (Position){.x = x, .y = y}))) {
				const Position p = {.x = x, .y = y};
				drawCell(p, o.color);
				if (o.drawOutline)
					drawOutline(p, o.outlineColor);
			}
		}
	}
}

void
drawBackground(State *s) {
	const Rectangle field = {
	    .x      = padding,
	    .y      = padding,
	    .width  = s->x * cellWidth,
	    .height = s->y * cellHeight,
	};
	ClearBackground(paddingColor);
	DrawRectangleRec(field, backgroundColor);
}

void
drawStatus(Status s) {
	const char *prefix   = "<<<";
	const char *suffix   = ">>>";
	const int   fontSize = padding;
	const int   spacing  = padding;
	int         offset   = padding;

	DrawText(prefix, offset, 0, fontSize, GRAY);
	offset += spacing + MeasureText(prefix, fontSize);

	const char *rule = ruleName(getRuleName(s.ruleSet));
	DrawText(rule, offset, 0, fontSize, WHITE);
	offset += spacing + MeasureText(rule, fontSize);
	if (s.paused) {
		const char *paused = "PAUSED";
		DrawText(paused, offset, 0, fontSize, YELLOW);
		offset += spacing + MeasureText(paused, fontSize);
	}
	if (s.drawFPS) {
		const int fpsLen = 32;
		char      fps[fpsLen];
		snprintf(fps, fpsLen, "FPS: %d (%d)", GetFPS(), s.targetFPS);
		DrawText(fps, offset, 0, fontSize, GREEN);
		offset += spacing + MeasureText(fps, fontSize);
	}
	if (s.drawOutline) {
		const char *outline = "OUTLINE";
		DrawText(outline, offset, 0, fontSize, RAYWHITE);
		offset += spacing + MeasureText(outline, fontSize);
	}
	if (s.drawTrail) {
		const char *trail = "TRAIL";
		DrawText(trail, offset, 0, fontSize, cellTrailColor);
		offset += spacing + MeasureText(trail, fontSize);
	}
	if (s.countCells) {
		const int nCellsLen = 32;
		char      nCells[nCellsLen];
		snprintf(nCells, nCellsLen, "CELLS: %d", s.numberOfCells);
		DrawText(nCells, offset, 0, fontSize, cellColor);
		offset += spacing + MeasureText(nCells, fontSize);
	}
	DrawText(suffix, offset, 0, fontSize, GRAY);
}

void
drawCursor(Position p) {
	drawOutline(p, cursorColor);
}

void
drawCell(Position p, Color c) {
	DrawRectangleRec(cellRectangle(p), c);
}

void
drawOutline(Position p, Color c) {
	DrawRectangleLinesEx(cellRectangle(p), 1, c);
}
