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

static void
updateStatus(Status *s) {
	if (IsKeyPressed(KEY_ESCAPE))
		s->paused = !s->paused;
	if (IsKeyPressed(KEY_F))
		s->drawFPS = !s->drawFPS;
	if (IsKeyPressed(KEY_C))
		s->countCells = !s->countCells;
	if (IsKeyPressed(KEY_R))
		changeRule(s->ruleSet);
}

static void
updateDrawOptions(DrawOptions *o) {
	if (IsKeyPressed(KEY_O))
		o->drawOutline = !o->drawOutline;
	if (IsKeyPressed(KEY_T))
		o->drawTrail = !o->drawTrail;
}

Position
cursorPosition() {
	return (Position){
		.x = (GetMouseX() - padding) / cellWidth,
		.y = (GetMouseY() - padding) / cellHeight,
	};
}

int
main(void) {
	const size_t   maxX         = screenWidth / cellWidth;
	const size_t   maxY         = screenHeight / cellHeight;
	const uint16_t screenWidth  = maxX * cellWidth + 2 * padding;
	const uint16_t screenHeight = maxY * cellHeight + 2 * padding;

	State  *s = newState(maxX, maxY);
	Status *t = &(Status){
		.paused        = false,
		.targetFPS     = defaultFPS,
		.drawFPS       = false,
		.numberOfCells = 0,
		.countCells    = true,
		.ruleSet       = newRuleSet(),

	};
	DrawOptions *o = &(DrawOptions){
		.cellColor       = cellColor,
		.outlineColor    = outlineColor,
		.trailColor      = trailColor,
		.backgroundColor = backgroundColor,
		.paddingColor    = paddingColor,
		.cursorColor     = cursorColor,
		.drawOutline     = false,
		.drawTrail       = false,
		.cellWidth       = cellWidth,
		.cellHeight      = cellHeight,
		.padding         = padding,
	};

	InitWindow(screenWidth, screenHeight, gameTitle);
	SetTargetFPS(t->targetFPS);
	SetExitKey(KEY_Q);
	HideCursor();

	while (!WindowShouldClose()) {
		BeginDrawing();
		updateStatus(t);
		updateDrawOptions(o);

		/* state */
		if (IsKeyPressed(KEY_SPACE))
			randomizeState(s);
		if (IsKeyDown(KEY_P))
			permutateState(s, maxX * maxY / ((maxX + maxY) / 2));
		if (IsKeyPressed(KEY_BACKSPACE))
			clearState(s);

		/* fps */
		if (IsKeyPressed(KEY_EQUAL)) {
			t->targetFPS += deltaFPS;
			SetTargetFPS(t->targetFPS);
		}
		if (IsKeyPressed(KEY_MINUS)) {
			t->targetFPS -= deltaFPS;
			if (t->targetFPS < minFPS)
				t->targetFPS = minFPS;
			SetTargetFPS(t->targetFPS);
		}

		/* drawing */
		Position p = cursorPosition();
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
			activateCell(s, p);
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
			disableCell(s, p);

		/* rendering */
		drawGame(s, t, o);
		drawCursor(p, o);
		if (!t->paused)
			updateState(s, getRule(t->ruleSet));
		if (t->countCells)
			t->numberOfCells = countCells(s);

		EndDrawing();
	}

	freeRuleSet(t->ruleSet);
	freeState(s);
	CloseWindow();
	return 0;
}
