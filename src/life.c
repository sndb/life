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
	State        *state;
	Status        status;
	TrailOpts     trail;
	CellOpts      cell;
	InterfaceOpts interface;
	GeometryOpts  geometry;
} Game;

Game
newGame() {
	const size_t maxX = (screenWidth - 2 * padding) / cellWidth;
	const size_t maxY = (screenHeight - 2 * padding) / cellHeight;
	return (Game){
		.state = newState(maxX, maxY),
		.status = {
			.paused        = false,
			.targetFPS     = defaultFPS,
			.drawFPS       = false,
			.numberOfCells = 0,
			.countCells    = false,
			.variation     = newVariation(),
		},
		.trail = {
			.drawTrail  = false,
			.trailColor = trailColor,
		},
		.cell = {
			.cellColor    = cellColor,
			.outlineColor = outlineColor,
			.drawOutline  = false,
		},
		.interface = {
			.backgroundColor = backgroundColor,
			.paddingColor    = paddingColor,
			.cursorColor     = cursorColor,
		},
		.geometry = {
			.cellWidth  = cellWidth,
			.cellHeight = cellHeight,
			.padding    = padding,
		},
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
handleInputTrailOpts(TrailOpts *t) {
	if (IsKeyPressed(KEY_T))
		t->drawTrail = !t->drawTrail;
}

static void
handleInputCellOpts(CellOpts *c) {
	if (IsKeyPressed(KEY_O))
		c->drawOutline = !c->drawOutline;
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
handleInput(Game *game) {
	handleInputStatus(&game->status);
	handleInputTrailOpts(&game->trail);
	handleInputCellOpts(&game->cell);
	handleInputState(game->state);
	handleInputDrawing(game->state, cursorPosition());
}

static void
preDrawGame(const Game *game) {
	drawBackground(game->state, &game->geometry, &game->interface);
	if (game->trail.drawTrail) {
		CellOpts cell  = game->cell;
		cell.cellColor = game->trail.trailColor;
		drawActiveCells(game->state, &game->geometry, &cell);
	}
}

static void
postDrawGame(const Game *game) {
	drawActiveCells(game->state, &game->geometry, &game->cell);
	drawStatus(&game->status, &game->geometry, &game->cell, &game->trail);
	drawCursor(cursorPosition(), &game->interface, &game->geometry);
}

static void
updateGame(Game *game) {
	if (!game->status.paused)
		updateState(game->state, game->status.variation->rule);
	if (game->status.countCells)
		game->status.numberOfCells = countCells(game->state);
}

static void
gameLoop(Game *game) {
	handleInput(game);
	BeginDrawing();
	preDrawGame(game);
	updateGame(game);
	postDrawGame(game);
	EndDrawing();
}

static void
init(Game *game) {
	InitWindow(screenWidth, screenHeight, gameTitle);
	SetTargetFPS(game->status.targetFPS);
	SetExitKey(KEY_Q);
	HideCursor();
}

static void
quit(Game *game) {
	freeVariation(game->status.variation);
	freeState(game->state);
	CloseWindow();
}

int
main(void) {
	Game game = newGame();
	init(&game);
	while (!WindowShouldClose())
		gameLoop(&game);
	quit(&game);
	return 0;
}
