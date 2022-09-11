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
	size_t       numberOfCells;
	bool         drawFPS;
	bool         countCells;
	bool         paused;
} Game;

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
		.state         = newState(maxX, maxY),
		.variation     = newVariation(),
		.geometry      = geometry,
		.mask          = DrawFill,
		.targetFPS     = defaultFPS,
		.numberOfCells = 0,
		.drawFPS       = false,
		.countCells    = false,
		.paused        = false,
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
handleInputGame(Game *g) {
	if (IsKeyPressed(KEY_ESCAPE))
		g->paused = !g->paused;
	if (IsKeyPressed(KEY_F))
		g->drawFPS = !g->drawFPS;
	if (IsKeyPressed(KEY_C))
		g->countCells = !g->countCells;
	if (IsKeyPressed(KEY_R))
		g->variation = g->variation->next;

	bool fpsUpdated = false;
	if (IsKeyPressed(KEY_EQUAL)) {
		g->targetFPS += deltaFPS;
		fpsUpdated = true;
	}
	if (IsKeyPressed(KEY_MINUS)) {
		g->targetFPS -= deltaFPS;
		fpsUpdated = true;
	}
	if (fpsUpdated) {
		if (g->targetFPS < minFPS)
			g->targetFPS = minFPS;
		SetTargetFPS(g->targetFPS);
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

	uint16_t o = drawElement(&g->geometry, 0, WHITE, g->variation->name);
	if (g->paused)
		o = drawElement(&g->geometry, o, BEIGE, "PAUSE");
	if (g->drawFPS)
		o = drawElement(&g->geometry, o, GREEN, "FPS %d/%d", GetFPS(), g->targetFPS);
	if (g->mask & DrawFill)
		o = drawElement(&g->geometry, o, cellColor, "FILL");
	if (g->mask & DrawOutline)
		o = drawElement(&g->geometry, o, GRAY, "OUTLINE");
	if (g->mask & DrawTrail)
		o = drawElement(&g->geometry, o, trailColor, "TRAIL");
	if (g->mask & DrawGlow)
		o = drawElement(&g->geometry, o, RED, "GLOW");
	if (g->countCells)
		o = drawElement(&g->geometry, o, LIME, "#CELLS %d", g->numberOfCells);
}

static void
updateGame(Game *g) {
	if (!g->paused)
		updateState(g->state, g->variation->rule);
	if (g->countCells)
		g->numberOfCells = countCells(g->state);
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
