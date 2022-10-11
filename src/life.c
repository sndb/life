#include "config.h"
#include "draw.h"
#include "raylib.h"
#include "rules.h"
#include "state.h"

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	State       *state;
	Variation   *variation;
	Geometry     geometry;
	DrawCellMask mask;
	uint16_t     targetFPS;
	size_t       generation;
	bool         paused;
} Game;

static_assert(sizeof(Game) == 40, "Game size is 40 bytes");

Game
newGame() {
	const size_t maxX     = (screenWidth - 2 * padding) / cellWidth;
	const size_t maxY     = (screenHeight - 2 * padding) / cellHeight;
	Geometry     geometry = {
		    .cellWidth  = cellWidth,
		    .cellHeight = cellHeight,
		    .padding    = padding,
		    .fontSize   = padding,
        };
	return (Game){
		.state     = newState(maxX, maxY),
		.variation = newVariation(),
		.geometry  = geometry,
		.mask      = DrawFill,
		.targetFPS = defaultFPS,
		.paused    = false,
	};
}

static Position
cursorPosition() {
	return (Position){
		.x = (GetMouseX() - padding) / cellWidth,
		.y = (GetMouseY() - padding) / cellHeight,
	};
}

static void
updateFPS(Game *g, int16_t delta) {
	g->targetFPS += delta;
	if (g->targetFPS < minFPS)
		g->targetFPS = minFPS;
	SetTargetFPS(g->targetFPS);
}

static void
handleInputGame(Game *g) {
	if (IsKeyPressed(KEY_ESCAPE))
		g->paused = !g->paused;
	else if (IsKeyPressed(KEY_R))
		g->variation = g->variation->next;
	else if (IsKeyPressed(KEY_EQUAL))
		updateFPS(g, deltaFPS);
	else if (IsKeyPressed(KEY_MINUS))
		updateFPS(g, -deltaFPS);
}

static void
handleInputDrawCellMask(DrawCellMask *m) {
	if (IsKeyPressed(KEY_T))
		*m ^= DrawTrail;
	else if (IsKeyPressed(KEY_O))
		*m ^= DrawOutline;
	else if (IsKeyPressed(KEY_G))
		*m ^= DrawGlow;
	else if (IsKeyPressed(KEY_L))
		*m ^= DrawFill;
}

static void
handleInputState(State *s) {
	if (IsKeyPressed(KEY_SPACE))
		randomizeState(s);
	else if (IsKeyDown(KEY_P))
		permutateState(s, s->x * s->y / ((s->x + s->y) / 2));
	else if (IsKeyPressed(KEY_BACKSPACE))
		clearState(s);
}

static void
handleInputDrawing(State *s, Position p) {
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		activateCell(s, p);
	else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
		disableCell(s, p);
}

static void
handleInput(Game *g) {
	handleInputGame(g);
	handleInputDrawCellMask(&g->mask);
	handleInputState(g->state);
	handleInputDrawing(g->state, cursorPosition());
}

static void
preDrawGame(const Game *g) {
	drawBackground(g->state, &g->geometry, paddingColor, backgroundColor);
	if (g->mask & DrawTrail)
		drawActiveCells(g->state, &g->geometry, trailColor, g->mask);
}

static void
postDrawGame(const Game *g) {
	drawActiveCells(g->state, &g->geometry, cellColor, g->mask);
	drawCursor(cursorPosition(), &g->geometry, cursorColor);

	uint16_t o = 0;

	o = drawElement(&g->geometry, o, WHITE, g->variation->name);
	o = drawElement(&g->geometry, o, GREEN, "FPS %d/%d", GetFPS(), g->targetFPS);
	o = drawElement(&g->geometry, o, LIME, "#CELLS %d", g->state->n);
	o = drawElement(&g->geometry, o, WHITE, "GEN %d", g->generation);
	if (g->paused)
		o = drawElement(&g->geometry, o, BEIGE, "PAUSE");
	if (g->mask & DrawFill)
		o = drawElement(&g->geometry, o, cellColor, "FILL");
	if (g->mask & DrawOutline)
		o = drawElement(&g->geometry, o, GRAY, "OUTLINE");
	if (g->mask & DrawTrail)
		o = drawElement(&g->geometry, o, trailColor, "TRAIL");
	if (g->mask & DrawGlow)
		o = drawElement(&g->geometry, o, RED, "GLOW");
}

static void
updateGame(Game *g) {
	if (g->paused)
		return;
	updateState(g->state, g->variation->rule);
	g->generation = g->state->n ? g->generation + 1 : 0;
}

static void
gameLoop(Game *g) {
	handleInput(g);
	BeginDrawing();
	preDrawGame(g);
	updateGame(g);
	postDrawGame(g);
	EndDrawing();
}

static void
init(Game *g) {
	InitWindow(screenWidth, screenHeight, gameTitle);
	SetTargetFPS(g->targetFPS);
	SetExitKey(KEY_Q);
	HideCursor();
}

static void
quit(Game *g) {
	freeVariation(g->variation);
	freeState(g->state);
	CloseWindow();
}

int
main(void) {
	Game g = newGame();
	init(&g);
	while (!WindowShouldClose())
		gameLoop(&g);
	quit(&g);
	return 0;
}
