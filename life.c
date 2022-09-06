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

static const int permutations = 1000;

Status
updateStatus(Status s) {
	if (IsKeyPressed(KEY_ESCAPE))
		s.paused = !s.paused;
	if (IsKeyPressed(KEY_F))
		s.drawFPS = !s.drawFPS;
	if (IsKeyPressed(KEY_O))
		s.drawOutline = !s.drawOutline;
	if (IsKeyPressed(KEY_T))
		s.drawTrail = !s.drawTrail;
	if (IsKeyPressed(KEY_C))
		s.countCells = !s.countCells;
	if (IsKeyPressed(KEY_R))
		changeRule(s.ruleSet);
	return s;
}

Position
cursorPosition() {
	int x = GetMouseX() - padding;
	int y = GetMouseY() - padding;
	return (Position){
	    .x = x / cellWidth,
	    .y = y / cellHeight,
	};
}

int
main(void) {
	const int maxX         = screenWidth / cellWidth;
	const int maxY         = screenHeight / cellHeight;
	const int screenWidth  = maxX * cellWidth + 2 * padding;
	const int screenHeight = maxY * cellHeight + 2 * padding;

	State *s = newState(maxX, maxY);
	Status t = (Status){
	    .paused        = false,
	    .targetFPS     = defaultFPS,
	    .drawFPS       = false,
	    .drawOutline   = false,
	    .drawTrail     = false,
	    .numberOfCells = 0,
	    .countCells    = true,
	    .ruleSet       = newRuleSet(),

	};

	InitWindow(screenWidth, screenHeight, gameTitle);
	SetTargetFPS(t.targetFPS);
	SetExitKey(KEY_Q);
	HideCursor();

	while (!WindowShouldClose()) {
		BeginDrawing();
		t = updateStatus(t);

		/* state */
		if (IsKeyPressed(KEY_SPACE))
			randomizeState(s);
		if (IsKeyDown(KEY_P))
			permutateState(s, permutations);
		if (IsKeyPressed(KEY_BACKSPACE))
			clearState(s);

		/* fps */
		if (IsKeyPressed(KEY_EQUAL)) {
			t.targetFPS += deltaFPS;
			SetTargetFPS(t.targetFPS);
		}
		if (IsKeyPressed(KEY_MINUS)) {
			t.targetFPS -= deltaFPS;
			if (t.targetFPS < minFPS)
				t.targetFPS = minFPS;
			SetTargetFPS(t.targetFPS);
		}

		/* drawing */
		Position p = cursorPosition();
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
			activateCell(s, p);
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
			disableCell(s, p);

		/* rendering */
		drawBackground(s);
		drawStatus(t);
		if (t.drawTrail)
			drawState(s,
			          (DrawOptions){
				      .color        = cellTrailColor,
				      .outlineColor = cellOutlineColor,
				      .drawOutline  = t.drawOutline,
				  });
		if (!t.paused)
			updateState(s, getRule(t.ruleSet));
		if (t.countCells)
			t.numberOfCells = countCells(s);
		drawState(s,
		          (DrawOptions){
			      .color        = cellColor,
			      .outlineColor = cellOutlineColor,
			      .drawOutline  = t.drawOutline,
			  });
		drawCursor(p);

		EndDrawing();
	}

	freeRuleSet(t.ruleSet);
	freeState(s);
	CloseWindow();
	return 0;
}
