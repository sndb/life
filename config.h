#ifndef CONFIG_H
#define CONFIG_H

#include <raylib.h>
#include <stdint.h>

static const char *gameTitle = "Conway's Game of Life";

/* resolution */
static const uint16_t screenWidth  = 1280;
static const uint16_t screenHeight = 720;
static const uint8_t  cellWidth    = 3;
static const uint8_t  cellHeight   = 3;
static const uint8_t  padding      = 10;

/* fps */
static const uint16_t defaultFPS = 30;
static const uint16_t deltaFPS   = 10;
static const uint16_t minFPS     = 10;

/* colors */
static const Color cellColor       = YELLOW;
static const Color trailColor      = BLUE;
static const Color outlineColor    = GRAY;
static const Color backgroundColor = BLACK;
static const Color paddingColor    = DARKGRAY;
static const Color cursorColor     = WHITE;

#endif
