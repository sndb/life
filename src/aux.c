#include "aux.h"

#include <assert.h>
#include <raylib.h>
#include <stdio.h>

void
die(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	exit(1);
}

uint8_t
ctoi(char c) {
	assert(c >= '0' && c <= '9');
	return c - '0';
}

bool
randBool() {
	return GetRandomValue(0, 1);
}
