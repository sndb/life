#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "raylib.h"

/* variables */
static const int maxX = screenWidth / cellWidth;
static const int maxY = screenHeight / cellHeight;
static const int permutations = 1000;
static const int ruleStopper = -1;
static const Rectangle field = {
    .x = padding,
    .y = padding,
    .width = maxX * cellWidth,
    .height = maxY * cellHeight,
};
static const char *rulesData[] = {
    "B1357/S1357",  "B2/S",         "B3/S012345678", "B3/S23",
    "B34/S34",      "B35678/S5678", "B36/S125",      "B36/S23",
    "B3678/S34678", "B368/S245",    "B4678/S35678",
};
static const int nRules = sizeof(rulesData) / sizeof(*rulesData);

/* types */
typedef enum {
  Replicator,
  Seeds,
  LifeWithoutDeath,
  Life,
  ThirtyFourLife,
  Diamoeba,
  TwoXTwo,
  HighLife,
  DayAndNight,
  Morley,
  Anneal,
} RuleName;

typedef struct {
  int x;
  int y;
} Position;

typedef bool Cell;

typedef Cell **State;

typedef struct {
  Color color;
  Color outlineColor;
  bool drawOutline;
} DrawOptions;

typedef struct {
  bool paused;
  int targetFPS;
  bool drawFPS;
  bool drawOutline;
  bool drawTrail;
  int numberOfCells;
  bool countCells;
  RuleName rule;
} Status;

typedef struct {
  int *born;
  int *survives;
} Rule;

/* headers */
void drawState(State s, DrawOptions o);
void drawBackground();
void drawStatus(Status s);
Cell inactiveCell();
Cell activeCell();
bool isActive(Cell c);
void updateState(State s, Rule r);
void permutateState(State s, int permutations);
void randomizeState(State s);
void clearState(State s);
void activateCell(State s, int x, int y);
void disableCell(State s, int x, int y);
int countCells(State s);
bool validLocation(int x, int y);
int neighbors(State s, int i, int j);
Position getMousePosition();
Rule readRule(const char *s);
void freeRule(Rule r);
Cell applyRule(Rule r, int neighbors, bool active);
char *ruleName(RuleName n);
Status updateStatus(Status s);

/* functions */
int ctoi(char c) { return c - '0'; }

void drawState(State s, DrawOptions o) {
  for (int x = 0; x < maxX; x++) {
    for (int y = 0; y < maxY; y++) {
      int xOffset = padding + cellWidth * x;
      int yOffset = padding + cellHeight * y;
      Rectangle rec = {xOffset, yOffset, cellWidth, cellHeight};
      if (isActive(s[x][y])) {
        DrawRectangleRec(rec, o.color);
        if (o.drawOutline) {
          DrawRectangleLinesEx(rec, 1, o.outlineColor);
        }
      }
    }
  }
}

void drawBackground() {
  ClearBackground(paddingColor);
  DrawRectangleRec(field, backgroundColor);
}

void drawStatus(Status s) {
  const int fontSize = padding;
  const int spacing = padding;
  int offset = padding;

  const char *rule = ruleName(s.rule);
  DrawText(rule, offset, 0, fontSize, WHITE);
  offset += spacing + MeasureText(rule, fontSize);
  if (s.paused) {
    const char *paused = "PAUSED";
    DrawText(paused, offset, 0, fontSize, YELLOW);
    offset += spacing + MeasureText(paused, fontSize);
  }
  if (s.drawFPS) {
    const int fpsLen = 32;
    char fps[fpsLen];
    snprintf(fps, fpsLen, "FPS: %d (%d)", GetFPS(), s.targetFPS);
    DrawText(fps, offset, 0, fontSize, GREEN);
    offset += spacing + MeasureText(fps, fontSize);
  }
  if (s.drawOutline) {
    const char *outline = "OUTLINE";
    DrawText(outline, offset, 0, fontSize, RAYWHITE);
    offset += spacing + MeasureText(outline, fontSize);
  }
  if (s.drawTrail) {
    const char *trail = "TRAIL";
    DrawText(trail, offset, 0, fontSize, cellTrailColor);
    offset += spacing + MeasureText(trail, fontSize);
  }
  if (s.countCells) {
    const int nCellsLen = 32;
    char nCells[nCellsLen];
    snprintf(nCells, nCellsLen, "CELLS: %d", s.numberOfCells);
    DrawText(nCells, offset, 0, fontSize, cellColor);
    offset += spacing + MeasureText(nCells, fontSize);
  }
}

Cell inactiveCell() { return false; }

Cell activeCell() { return true; }

bool isActive(Cell c) { return c; }

State createState() {
  State s = malloc(maxX * sizeof(Cell *));
  for (int x = 0; x < maxX; x++) {
    s[x] = malloc(maxY * sizeof(Cell));
    for (int y = 0; y < maxY; y++) {
      s[x][y] = inactiveCell();
    }
  }
  return s;
}

void copyState(State dst, State src) {
  for (int x = 0; x < maxX; x++) {
    for (int y = 0; y < maxY; y++) {
      dst[x][y] = src[x][y];
    }
  }
}

void freeState(State s) {
  for (int x = 0; x < maxX; x++) {
    free(s[x]);
  }
  free(s);
}

void updateState(State s, Rule r) {
  State next = createState();
  for (int x = 0; x < maxX; x++) {
    for (int y = 0; y < maxY; y++) {
      next[x][y] = applyRule(r, neighbors(s, x, y), isActive(s[x][y]));
    }
  }
  copyState(s, next);
  freeState(next);
}

void permutateState(State s, int permutations) {
  for (int i = 0; i < permutations; i++) {
    int x = GetRandomValue(0, maxX - 1);
    int y = GetRandomValue(0, maxY - 1);
    s[x][y] = activeCell();
  }
}

void randomizeState(State s) {
  for (int x = 0; x < maxX; x++) {
    for (int y = 0; y < maxY; y++) {
      s[x][y] = GetRandomValue(0, 1) ? activeCell() : inactiveCell();
    }
  }
}

void clearState(State s) {
  for (int x = 0; x < maxX; x++) {
    for (int y = 0; y < maxY; y++) {
      s[x][y] = inactiveCell();
    }
  }
}

void activateCell(State s, int x, int y) {
  if (validLocation(x, y)) {
    s[x][y] = activeCell();
  }
}

void disableCell(State s, int x, int y) {
  if (validLocation(x, y)) {
    s[x][y] = inactiveCell();
  }
}

int countCells(State s) {
  int c = 0;
  for (int x = 0; x < maxX; x++) {
    for (int y = 0; y < maxY; y++) {
      if (isActive(s[x][y])) {
        c++;
      }
    }
  }
  return c;
}

bool validLocation(int x, int y) {
  return x >= 0 && x < maxX && y >= 0 && y < maxY;
}

int neighbors(State s, int i, int j) {
  int c = 0;
  for (int x = -1; x <= 1; x++) {
    for (int y = -1; y <= 1; y++) {
      int p = i + x, q = j + y;
      if ((x != 0 || y != 0) && validLocation(p, q) && isActive(s[p][q])) {
        c++;
      }
    }
  }
  return c;
}

Position getMousePosition() {
  int x = GetMouseX() - padding;
  int y = GetMouseY() - padding;
  return (Position){
      .x = x / cellWidth,
      .y = y / cellHeight,
  };
}

Rule readRule(const char *r) {
  const char largestInput[] = "B012345678/S012345678";
  const int maxLen = sizeof(largestInput) / sizeof(*largestInput);
  const int nLen = 10;
  int *born = malloc(nLen * sizeof(int));
  int *survives = malloc(nLen * sizeof(int));
  int state = 0, j = 0, k = 0;
  for (int i = 0; i < maxLen; i++) {
    const char c = r[i];
    printf("%c %d\n", c, state);
    switch (state) {
    case 0: /* B */
      assert(c == 'B');
      state++;
      break;
    case 1: /* number after B */
      if (isdigit(c)) {
        born[j] = ctoi(c);
        j++;
      } else {
        born[j] = ruleStopper;
        i--;
        state++;
      }
      break;
    case 2: /* / */
      assert(c == '/');
      state++;
      break;
    case 3: /* S */
      assert(c == 'S');
      state++;
      break;
    case 4: /* number after S */
      if (isdigit(c)) {
        survives[k] = ctoi(c);
        k++;
      } else {
        survives[k] = ruleStopper;
        goto outerEnd;
      }
      break;
    default:
      fprintf(stderr, "unknown state %d", state);
      exit(1);
    }
  }
outerEnd:
  return (Rule){.born = born, .survives = survives};
}

void freeRule(Rule r) {
  free(r.born);
  free(r.survives);
}

Cell applyRule(Rule r, int neighbors, bool active) {
  if (active) {
    for (int i = 0; r.survives[i] != ruleStopper; i++) {
      if (neighbors == r.survives[i]) {
        return true;
      }
    }
  } else {
    for (int i = 0; r.born[i] != ruleStopper; i++) {
      if (neighbors == r.born[i]) {
        return true;
      }
    }
  }
  return false;
}

char *ruleName(RuleName n) {
  switch (n) {
  case Replicator:
    return "Replicator";
  case Seeds:
    return "Seeds";
  case LifeWithoutDeath:
    return "Life without Death";
  case Life:
    return "Life";
  case ThirtyFourLife:
    return "34 Life";
  case Diamoeba:
    return "Diamoeba";
  case TwoXTwo:
    return "2x2";
  case HighLife:
    return "HighLife";
  case DayAndNight:
    return "Day & Night";
  case Morley:
    return "Morley";
  case Anneal:
    return "Anneal";
  }
  return "Unknown";
}

Status updateStatus(Status s) {
  if (IsKeyPressed(KEY_ESCAPE)) {
    s.paused = !s.paused;
  }
  if (IsKeyPressed(KEY_F)) {
    s.drawFPS = !s.drawFPS;
  }
  if (IsKeyPressed(KEY_O)) {
    s.drawOutline = !s.drawOutline;
  }
  if (IsKeyPressed(KEY_T)) {
    s.drawTrail = !s.drawTrail;
  }
  if (IsKeyPressed(KEY_C)) {
    s.countCells = !s.countCells;
  }
  if (IsKeyPressed(KEY_R)) {
    s.rule = (s.rule + 1) % nRules;
  }
  return s;
}

int main(void) {
  const int screenWidth = maxX * cellWidth + 2 * padding;
  const int screenHeight = maxY * cellHeight + 2 * padding;
  State s = createState();
  Status t = (Status){
      .paused = false,
      .targetFPS = defaultFPS,
      .drawFPS = false,
      .drawOutline = false,
      .drawTrail = false,
      .numberOfCells = 0,
      .countCells = true,
      .rule = Life,
  };
  Rule rules[nRules];
  for (int i = 0; i < nRules; i++) {
    rules[i] = readRule(rulesData[i]);
  }

  InitWindow(screenWidth, screenHeight, gameTitle);
  SetTargetFPS(t.targetFPS);
  SetExitKey(KEY_Q);

  while (!WindowShouldClose()) {
    BeginDrawing();
    t = updateStatus(t);

    /* state */
    if (IsKeyPressed(KEY_SPACE)) {
      randomizeState(s);
    }
    if (IsKeyDown(KEY_P)) {
      permutateState(s, permutations);
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
      clearState(s);
    }

    /* fps */
    if (IsKeyPressed(KEY_EQUAL)) {
      t.targetFPS += fpsIncrement;
      SetTargetFPS(t.targetFPS);
    }
    if (IsKeyPressed(KEY_MINUS)) {
      t.targetFPS -= fpsIncrement;
      if (t.targetFPS < minFPS) {
        t.targetFPS = minFPS;
      }
      SetTargetFPS(t.targetFPS);
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
    drawStatus(t);
    if (t.drawTrail) {
      drawState(s, (DrawOptions){
                       .color = cellTrailColor,
                       .outlineColor = cellOutlineColor,
                       .drawOutline = t.drawOutline,
                   });
    }
    if (!t.paused) {
      updateState(s, rules[t.rule]);
    }
    if (t.countCells) {
      t.numberOfCells = countCells(s);
    }
    drawState(s, (DrawOptions){
                     .color = cellColor,
                     .outlineColor = cellOutlineColor,
                     .drawOutline = t.drawOutline,
                 });

    EndDrawing();
  }

  for (int i = 0; i < nRules; i++) {
    freeRule(rules[i]);
  }
  freeState(s);
  CloseWindow();
  return 0;
}
