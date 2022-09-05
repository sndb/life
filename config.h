#include "raylib.h"

static const char *gameTitle = "Conway's Game of Life";

/* resolution */
static const int screenWidth  = 1280;
static const int screenHeight = 720;
static const int cellWidth    = 3;
static const int cellHeight   = 3;
static const int padding      = 10;

/* fps */
static const int defaultFPS   = 30;
static const int fpsIncrement = 10;
static const int minFPS       = 10;

/* colors */
static const Color cellColor        = YELLOW;
static const Color cellTrailColor   = BLUE;
static const Color cellOutlineColor = GRAY;
static const Color paddingColor     = DARKGRAY;
static const Color backgroundColor  = BLACK;
static const Color cursorColor      = WHITE;
