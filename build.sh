#!/bin/sh

GAME_NAME="life"
SOURCES="*.c"
CC=cc
FLAGS="-std=c11 -Wpedantic -Wall -Wextra -O3"

$CC -lraylib $FLAGS -o $GAME_NAME $SOURCES
