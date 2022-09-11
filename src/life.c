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
	Status       status;
	Geometry     geometry;
	DrawCellMask mask;
} Game;

Game
newGame() {
	const size_t maxX   = (screenWidth - 2 * padding) / cellWidth;
	const size_t maxY   = (screenHeight - 2 * padding) / cellHeight;
	State       *state  = newState(maxX, maxY);
	Status       status = {
		      .paused        = false,
		      .targetFPS     = defaultFPS,
		      .drawFPS       = false,
		      .numberOfCells = 0,
		      .countCells    = false,
		      .variation     = newVariation(),
        };
	Geometry geometry = {
		.cellWidth  = cellWidth,
		.cellHeight = cellHeight,
		.padding    = padding,
		.fontSize   = padding,
	};
	DrawCellMask mask = DrawFill;
	return (Game){
		.state    = state,
		.status   = status,
		.geometry = geometry,
		.mask     = mask,
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
handleInputStatus(Status *s) {
	if (IsKeyPressed(KEY_ESCAPE))
		s->paused = !s->paused;
	if (IsKeyPressed(KEY_F))
		s->drawFPS = !s->drawFPS;
	if (IsKeyPressed(KEY_C))
		s->countCells = !s->countCells;
	if (IsKeyPressed(KEY_R))
		s->variation = s->variation->next;

	bool fpsUpdated = false;
	if (IsKeyPressed(KEY_EQUAL)) {
		s->targetFPS += deltaFPS;
		fpsUpdated = true;
	}
	if (IsKeyPressed(KEY_MINUS)) {
		s->targetFPS -= deltaFPS;
		fpsUpdated = true;
	}
	if (fpsUpdated) {
		if (s->targetFPS < minFPS)
			s->targetFPS = minFPS;
		SetTargetFPS(s->targetFPS);
	}
}

static void
handleInputDrawCellMask(DrawCellMask *m) {
	if (IsKeyPressed(KEY_T))
		*m ^= DrawTrail;
	if (IsKeyPressed(KEY_O))
		*m ^= DrawOutline;
	if (IsKeyPressed(KEY_G))
		*m ^= DrawGlow;
	if (IsKeyPressed(KEY_L))
		*m ^= DrawFill;
}

static void
handleInputState(State *s) {
	if (IsKeyPressed(KEY_SPACE))
		randomizeState(s);
	if (IsKeyDown(KEY_P))
		permutateState(s, s->x * s->y / ((s->x + s->y) / 2));
	if (IsKeyPressed(KEY_BACKSPACE))
		clearState(s);
}

static void
handleInputDrawing(State *s, Position p) {
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		activateCell(s, p);
	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
		disableCell(s, p);
}

static void
handleInput(Game *g) {
	handleInputStatus(&g->status);
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

	uint16_t o = drawElement(&g->geometry, 0, WHITE, g->status.variation->name);
	if (g->status.paused)
		o = drawElement(&g->geometry, o, BEIGE, "PAUSE");
	if (g->status.drawFPS)
		o = drawElement(&g->geometry, o, GREEN, "FPS %d/%d", GetFPS(), g->status.targetFPS);
	if (g->mask & DrawFill)
		o = drawElement(&g->geometry, o, cellColor, "FILL");
	if (g->mask & DrawOutline)
		o = drawElement(&g->geometry, o, GRAY, "OUTLINE");
	if (g->mask & DrawTrail)
		o = drawElement(&g->geometry, o, trailColor, "TRAIL");
	if (g->mask & DrawGlow)
		o = drawElement(&g->geometry, o, RED, "GLOW");
	if (g->status.countCells)
		o = drawElement(&g->geometry, o, LIME, "#CELLS %d", g->status.numberOfCells);
}

static void
updateGame(Game *g) {
	if (!g->status.paused)
		updateState(g->state, g->status.variation->rule);
	if (g->status.countCells)
		g->status.numberOfCells = countCells(g->state);
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
	SetTargetFPS(g->status.targetFPS);
	SetExitKey(KEY_Q);
	HideCursor();
}

static void
quit(Game *g) {
	freeVariation(g->status.variation);
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
