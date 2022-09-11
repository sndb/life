#include "draw.h"

#include "aux.h"
#include "cell.h"

#include <assert.h>
#include <raylib.h>
#include <raymath.h>
#include <stdarg.h>
#include <stdio.h>

static Rectangle
cellRectangle(Position p, const Geometry *g) {
	return (Rectangle){
		.x      = g->padding + g->cellWidth * p.x,
		.y      = g->padding + g->cellHeight * p.y,
		.width  = g->cellWidth,
		.height = g->cellHeight,
	};
}

static Rectangle
fieldRectangle(const State *s, const Geometry *g) {
	return (Rectangle){
		.x      = g->padding,
		.y      = g->padding,
		.width  = s->x * g->cellWidth,
		.height = s->y * g->cellHeight,
	};
}

void
drawActiveCells(const State *s, const Geometry *g, Color fg, DrawCellMask m) {
	for (size_t x = 0; x < s->x; x++) {
		for (size_t y = 0; y < s->y; y++) {
			const Position pos = {.x = x, .y = y};
			if (!isActive(getCell(s, pos)))
				continue;
			const Rectangle rec = cellRectangle(pos, g);
			if (m & DrawFill)
				DrawRectangleRec(rec, fg);
			if (m & DrawOutline) {
				Vector3 hsv = ColorToHSV(fg);
				hsv.z       = Clamp(hsv.z - 0.2, 0, 1);
				DrawRectangleLinesEx(cellRectangle(pos, g),
				                     1,
				                     ColorFromHSV(hsv.x, hsv.y, hsv.z));
			}
			if (m & DrawGlow)
				DrawCircleGradient(rec.x + rec.width / 2,
				                   rec.y + rec.height / 2,
				                   (rec.width + rec.height) / 2.0 * 1.5,
				                   Fade(fg, 1.0 / 3),
				                   Fade(fg, 0.1 / 3));
		}
	}
}

void
drawBackground(const State *s, const Geometry *g, Color padding, Color bg) {
	ClearBackground(padding);
	DrawRectangleRec(fieldRectangle(s, g), bg);
}

uint16_t
drawElement(const Geometry *g, uint16_t offset, Color fg, const char *fmt, ...) {
	va_list ap;
	int     n;

	va_start(ap, fmt);
	n = vsnprintf(NULL, 0, fmt, ap);
	va_end(ap);
	assert(n >= 0);

	size_t size = n + 1;
	char  *str  = malloc(size);
	assert(str);

	va_start(ap, fmt);
	n = vsnprintf(str, size, fmt, ap);
	va_end(ap);
	assert(n >= 0);

	DrawText(str, g->padding + offset, 0, g->fontSize, fg);
	offset += g->padding + MeasureText(str, g->fontSize);

	free(str);
	return offset;
}

void
drawCursor(Position p, const Geometry *g, Color fg) {
	DrawRectangleLinesEx(cellRectangle(p, g), 1, fg);
}
