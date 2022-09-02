#include <stdlib.h>
#include <string.h>

#include "raylib.h"

/* constants */
#define TITLE "Conway's Game of Life"
#define FPS 30
#define FPS_INCREMENT 10
#define MIN_FPS 10
#define MAX_X (int)(1280 / CELL_WIDTH)
#define MAX_Y (int)(720 / CELL_HEIGHT)
#define CELL_WIDTH 3
#define CELL_HEIGHT 3
#define CELL_COLOR YELLOW
#define CELL_BORDER_COLOR Fade(BLACK, 0.75)
#define PADDING 10
#define PADDING_COLOR DARKGRAY
#define BACKGROUND_COLOR BLACK
#define PERMUTATIONS 1000

/* globals */
static KeyboardKey exitKeys[] = {KEY_Q};
static Rectangle screen = {
    .x = PADDING,
    .y = PADDING,
    .width = MAX_X * CELL_WIDTH,
    .height = MAX_Y * CELL_HEIGHT,
};

/* types */
typedef struct {
  int x;
  int y;
} Position;

typedef struct {
  bool active;
} Cell;

typedef struct {
  Cell cells[MAX_X][MAX_Y];
} State;

typedef struct {
  bool borders;
  Color color;
  Color borderColor;
} DrawOptions;

/* headers */
void drawState(State *s, DrawOptions o);
void drawBackground();
Cell inactiveCell();
Cell activeCell();
void updateState(State *s);
void permutateState(State *s, int permutations);
void randomizeState(State *s);
void clearState(State *s);
void activateCell(State *s, int x, int y);
void disableCell(State *s, int x, int y);
bool validLocation(int x, int y);
int neighbors(State *s, int i, int j);
bool isExitKeyPressed();
Position getMousePosition();

/* functions */
void drawState(State *s, DrawOptions o) {
  for (int x = 0; x < MAX_X; x++) {
    for (int y = 0; y < MAX_Y; y++) {
      int xOffset = PADDING + CELL_WIDTH * x;
      int yOffset = PADDING + CELL_HEIGHT * y;
      Rectangle rec = {xOffset, yOffset, CELL_WIDTH, CELL_HEIGHT};
      if (s->cells[x][y].active) {
        DrawRectangleRec(rec, o.color);
        if (o.borders) {
          DrawRectangleLinesEx(rec, 1, o.borderColor);
        }
      }
    }
  }
}

void drawBackground() {
  ClearBackground(PADDING_COLOR);
  DrawRectangleRec(screen, BACKGROUND_COLOR);
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
    s->cells[x][y] = activeCell();
  }
}

void randomizeState(State *s) {
  for (int x = 0; x < MAX_X; x++) {
    for (int y = 0; y < MAX_Y; y++) {
      s->cells[x][y] = GetRandomValue(0, 1) ? activeCell() : inactiveCell();
    }
  }
}

void clearState(State *s) {
  for (int x = 0; x < MAX_X; x++) {
    for (int y = 0; y < MAX_Y; y++) {
      s->cells[x][y] = inactiveCell();
    }
  }
}

void activateCell(State *s, int x, int y) {
  if (validLocation(x, y)) {
    s->cells[x][y].active = true;
  }
}

void disableCell(State *s, int x, int y) {
  if (validLocation(x, y)) {
    s->cells[x][y].active = false;
  }
}

bool validLocation(int x, int y) {
  return x >= 0 && x < MAX_X && y >= 0 && y < MAX_Y;
}

int neighbors(State *s, int i, int j) {
  int c = 0;
  for (int x = -1; x <= 1; x++) {
    for (int y = -1; y <= 1; y++) {
      int p = i + x, q = j + y;
      if ((x != 0 || y != 0) && validLocation(p, q) && s->cells[p][q].active) {
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

Position getMousePosition() {
  int x = GetMouseX() - PADDING;
  int y = GetMouseY() - PADDING;
  return (Position){
      .x = x / CELL_WIDTH,
      .y = y / CELL_HEIGHT,
  };
}

int main(void) {
  const int screenWidth = MAX_X * CELL_WIDTH + 2 * PADDING;
  const int screenHeight = MAX_Y * CELL_HEIGHT + 2 * PADDING;
  State *s = create_state();
  bool borders = false;
  bool paused = false;
  bool drawPreviousState = false;
  int fps = FPS;

  InitWindow(screenWidth, screenHeight, TITLE);
  SetTargetFPS(fps);
  SetExitKey(KEY_NULL);

  while (!WindowShouldClose()) {
    BeginDrawing();

    if (isExitKeyPressed()) {
      break;
    }

    /* options */
    if (IsKeyPressed(KEY_B)) {
      borders = !borders;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
      paused = !paused;
    }
    if (IsKeyPressed(KEY_APOSTROPHE)) {
      drawPreviousState = !drawPreviousState;
    }

    /* state */
    if (IsKeyPressed(KEY_SPACE)) {
      randomizeState(s);
    }
    if (IsKeyDown(KEY_P)) {
      permutateState(s, PERMUTATIONS);
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
      clearState(s);
    }

    /* fps */
    if (IsKeyPressed(KEY_EQUAL)) {
      fps += FPS_INCREMENT;
      SetTargetFPS(fps);
    }
    if (IsKeyPressed(KEY_MINUS)) {
      fps -= FPS_INCREMENT;
      if (fps < MIN_FPS) {
        fps = MIN_FPS;
      }
      SetTargetFPS(fps);
    }

    /* drawing */
    Position p = getMousePosition();
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      activateCell(s, p.x, p.y);
    }
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
      disableCell(s, p.x, p.y);
    }

    /* rendering */
    drawBackground();
    if (drawPreviousState) {
      drawState(s, (DrawOptions){
                       .color = BLUE,
                       .borderColor = CELL_BORDER_COLOR,
                       .borders = borders,
                   });
    }
    if (!paused) {
      updateState(s);
    }
    drawState(s, (DrawOptions){
                     .color = CELL_COLOR,
                     .borderColor = CELL_BORDER_COLOR,
                     .borders = borders,
                 });

    EndDrawing();
  }

  free(s);
  CloseWindow();
  return 0;
}
