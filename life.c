#include "aux.h"
#include "config.h"
#include "field.h"
#include "preprocess.h"
#include "rules.h"

#include <assert.h>
#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define DRAWFILL	(1 << 0)
#define DRAWOUTLINE	(1 << 1)
#define DRAWGLOW	(1 << 2)

static const char *title = "life";

struct game {
	struct variation variation;
	struct field *field;
	size_t generation;
	size_t ncells;
	uint32_t rule;
	uint16_t targetFPS;
	uint8_t style;
	bool paused;
};

static_assert(sizeof(struct game) == 48, "48 byte game size");

static struct position
cursorPosition()
{
	return (struct position){
		.x = (GetMouseX() - padding) / cellWidth,
		.y = (GetMouseY() - padding) / cellHeight,
	};
}

static struct game
gameCreate()
{
	const size_t x = (screenWidth - 2 * padding) / cellWidth;
	const size_t y = (screenHeight - 2 * padding) / cellHeight;
	struct variation variation = getVariation();
	return (struct game){
		.field = fieldCreate(x, y),
		.variation = variation,
		.rule = parseCode(variation.code),
		.style = DRAWFILL,
		.targetFPS = defaultFPS,
		.paused = false,
	};
}

static void
updateFPS(struct game *g, int16_t delta)
{
	g->targetFPS += delta;
	if (g->targetFPS < minFPS)
		g->targetFPS = minFPS;
	SetTargetFPS(g->targetFPS);
}

static void
processInput(struct game *g)
{
	const struct position p = cursorPosition();
	if (IsKeyPressed(KEY_ESCAPE))
		g->paused = !g->paused;
	else if (IsKeyPressed(KEY_R)) {
		g->variation = getVariation();
		g->rule = parseCode(g->variation.code);
	} else if (IsKeyPressed(KEY_EQUAL))
		updateFPS(g, deltaFPS);
	else if (IsKeyPressed(KEY_MINUS))
		updateFPS(g, -deltaFPS);
	else if (IsKeyPressed(KEY_O))
		g->style ^= DRAWOUTLINE;
	else if (IsKeyPressed(KEY_G))
		g->style ^= DRAWGLOW;
	else if (IsKeyPressed(KEY_L))
		g->style ^= DRAWFILL;
	else if (IsKeyPressed(KEY_SPACE))
		fieldRandomize(g->field);
	else if (IsKeyDown(KEY_P))
		fieldPermutate(g->field, g->field->x + g->field->y);
	else if (IsKeyPressed(KEY_BACKSPACE))
		fieldClear(g->field);
	else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		g->field->matrix[matrixPos(g->field, p.x, p.y)] = 1;
	else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
		g->field->matrix[matrixPos(g->field, p.x, p.y)] = 0;
}

static Rectangle
cellRectangle(size_t x, size_t y)
{
	return (Rectangle){
		.x = padding + cellWidth * x,
		.y = padding + cellHeight * y,
		.width = cellWidth,
		.height = cellHeight,
	};
}

static Rectangle
fieldRectangle(const struct field *f)
{
	return (Rectangle){
		.x = padding,
		.y = padding,
		.width = f->x * cellWidth,
		.height = f->y * cellHeight,
	};
}

static void
drawElement(uint16_t *offset, Color col, const char *fmt, ...)
{
	char s[1 << 8] = {0};
	va_list ap;
	va_start(ap, fmt);
	int n = vsnprintf(s, 1 << 8, fmt, ap);
	va_end(ap);
	assert(n >= 0);
	DrawText(s, padding + *offset, 0, fontSize, col);
	*offset += padding + MeasureText(s, fontSize);
}

static void
drawGame(struct game *g)
{
	g->ncells = 0;
	ClearBackground(paddingColor);
	DrawRectangleRec(fieldRectangle(g->field), backgroundColor);
	for (size_t i = 0; i < g->field->x * g->field->y; i++) {
		const size_t x = i % g->field->x;
		const size_t y = i / g->field->x;

		if (!g->field->matrix[i])
			continue;
		g->ncells++;
		const Rectangle r = cellRectangle(x, y);
		if (g->style & DRAWFILL)
			DrawRectangleRec(r, cellColor);
		if (g->style & DRAWOUTLINE)
			DrawRectangleLinesEx(r, 1, outlineColor);
		if (g->style & DRAWGLOW)
			DrawCircleGradient(r.x + r.width / 2, r.y + r.height / 2, glowRadius, glowColor1, glowColor2);
	}
	DrawRectangleLinesEx(cellRectangle(cursorPosition().x, cursorPosition().y), 1, cursorColor);
	uint16_t off = 0;
	drawElement(&off, WHITE, g->variation.name);
	drawElement(&off, GREEN, "FPS %d/%d", GetFPS(), g->targetFPS);
	drawElement(&off, LIME, "#CELLS %d", g->ncells);
	drawElement(&off, WHITE, "GEN %d", g->generation);
	if (g->paused)
		drawElement(&off, BEIGE, "PAUSE");
	if (g->style & DRAWFILL)
		drawElement(&off, cellColor, "FILL");
	if (g->style & DRAWOUTLINE)
		drawElement(&off, GRAY, "OUTLINE");
	if (g->style & DRAWGLOW)
		drawElement(&off, RED, "GLOW");
}

int
main(void)
{
	struct game g = gameCreate();
	InitWindow(screenWidth, screenHeight, title);
	SetTargetFPS(g.targetFPS);
	SetExitKey(KEY_Q);
	HideCursor();
	while (!WindowShouldClose()) {
		processInput(&g);
		BeginDrawing();
		drawGame(&g);
		EndDrawing();
		if (!g.paused) {
			fieldPreprocess(g.field);
			fieldCompile(g.field, g.rule);
			g.generation = g.ncells ? g.generation + 1 : 0;
		}
	}
	free(g.field);
	CloseWindow();
}
