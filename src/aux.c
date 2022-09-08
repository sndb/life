#include "aux.h"

#include <assert.h>
#include <raylib.h>

uint8_t
ctoi(char c) {
	assert(c >= '0' && c <= '9');
	return c - '0';
}

bool
randBool() {
	return GetRandomValue(0, 1);
}
