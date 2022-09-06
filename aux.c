#include "aux.h"

#include <raylib.h>

int
ctoi(char c) {
	return c - '0';
}

bool
randBool() {
	return GetRandomValue(0, 1);
}
