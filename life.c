#include <stdlib.h>
#include <string.h>

#include "raylib.h"

/* constants */
#define TITLE "Conway's Game of Life"
#define FPS 12
#define MAX_X (int)(1280 / CELL_WIDTH)
#define MAX_Y (int)(720 / CELL_HEIGHT)
#define CELL_WIDTH 3
#define CELL_HEIGHT 3
#define CELL_COLOR YELLOW
#define CELL_BORDER_COLOR Fade(BLACK, 0.75)
#define DRAW_CELL_BORDER false
#define PADDING 10
#define PADDING_COLOR DARKGRAY
#define BACKGROUND_COLOR BLACK
#define PERMUTATIONS 1000

/* globals */
KeyboardKey exitKeys[] = {KEY_ESCAPE, KEY_Q, KEY_BACKSPACE, KEY_DELETE};

/* types */
typedef struct {
  bool active;
} Cell;

typedef struct {
  Cell cells[MAX_X][MAX_Y];
} State;

/* headers */
void drawState(State *s);
Cell inactiveCell();
Cell activeCell();
void updateState(State *s);
void permutateState(State *s, int permutations);
void randomizeState(State *s);
bool validLocation(Vector2 p);
int neighbors(State *s, int i, int j);
bool isExitKeyPressed();

/* functions */
void drawState(State *s) {
  ClearBackground(PADDING_COLOR);
  Rectangle screen = {
      PADDING,
      PADDING,
      MAX_X * CELL_WIDTH,
      MAX_Y * CELL_HEIGHT,
  };
  DrawRectangleRec(screen, BACKGROUND_COLOR);
  for (int x = 0; x < MAX_X; x++) {
    int xOffset = PADDING + CELL_WIDTH * x;
    for (int y = 0; y < MAX_Y; y++) {
      int yOffset = PADDING + CELL_HEIGHT * y;
      Rectangle rec = {xOffset, yOffset, CELL_WIDTH, CELL_HEIGHT};
      if (s->cells[x][y].active) {
        DrawRectangleRec(rec, CELL_COLOR);
        if (DRAW_CELL_BORDER) {
          DrawRectangleLinesEx(rec, 1, CELL_BORDER_COLOR);
        }
      }
    }
  }
}

Cell inactiveCell() {
  Cell c = {false};
  return c;
}

Cell activeCell() {
  Cell c = {true};
  return c;
}

State *create_state() {
  State *s = malloc(sizeof(State));
  for (int x = 0; x < MAX_X; x++) {
    for (int y = 0; y < MAX_Y; y++) {
      s->cells[x][y] = inactiveCell();
    }
  }
  return s;
}

void updateState(State *s) {
  State *next = create_state();
  for (int x = 0; x < MAX_X; x++) {
    for (int y = 0; y < MAX_Y; y++) {
      int n = neighbors(s, x, y);
      if ((n == 2 || n == 3) && (s->cells[x][y].active)) {
        next->cells[x][y].active = true;
      } else if (n == 3 && !s->cells[x][y].active) {
        next->cells[x][y].active = true;
      }
    }
  }
  *s = *next;
  free(next);
}

void permutateState(State *s, int permutations) {
  for (int i = 0; i < permutations; i++) {
    int x = GetRandomValue(0, MAX_X - 1);
    int y = GetRandomValue(0, MAX_Y - 1);
    s->cells[x][y].active = true;
  }
}

void randomizeState(State *s) {
  for (int x = 0; x < MAX_X; x++) {
    for (int y = 0; y < MAX_Y; y++) {
      s->cells[x][y] = GetRandomValue(0, 1) ? activeCell() : inactiveCell();
    }
  }
}

bool validLocation(Vector2 p) {
  return p.x >= 0 && p.x < MAX_X && p.y >= 0 && p.y < MAX_Y;
}

int neighbors(State *s, int i, int j) {
  int c = 0;
  for (int x = -1; x <= 1; x++) {
    for (int y = -1; y <= 1; y++) {
      int p = i + x, q = j + y;
      if (validLocation((Vector2){p, q}) && s->cells[p][q].active) {
        c++;
      }
    }
  }
  return c;
}

bool isExitKeyPressed() {
  for (size_t i = 0; i < sizeof(exitKeys) / sizeof(*exitKeys); i++) {
    if (IsKeyPressed(exitKeys[i])) {
      return true;
    }
  }
  return false;
}

int main(void) {
  const int screenWidth = MAX_X * CELL_WIDTH + 2 * PADDING;
  const int screenHeight = MAX_Y * CELL_HEIGHT + 2 * PADDING;
  State *s = create_state();

  InitWindow(screenWidth, screenHeight, TITLE);
  SetTargetFPS(FPS);
  SetExitKey(KEY_NULL);

  while (!WindowShouldClose()) {
    BeginDrawing();
    {
      if (isExitKeyPressed()) {
        break;
      }
      if (IsKeyPressed(KEY_SPACE)) {
        randomizeState(s);
      }
      if (IsKeyDown(KEY_P)) {
        permutateState(s, PERMUTATIONS);
      }
      drawState(s);
      updateState(s);
    }
    EndDrawing();
  }

  free(s);
  CloseWindow();
  return 0;
}
