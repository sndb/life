#ifndef AUX_H
#define AUX_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
	size_t x;
	size_t y;
} Position;

static_assert(sizeof(Position) == 16, "Position size is 16 bytes");

/* crash with an error */
void die(const char *fmt, ...);

/* converts a digit to a number */
uint8_t ctoi(char c);

/* returns a random boolean value */
bool randBool();

#endif
