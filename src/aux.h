#ifndef AUX_H
#define AUX_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
	size_t x;
	size_t y;
} Position;

/* crash with an error */
void die(const char *fmt, ...);

/* converts a digit to a number */
uint8_t ctoi(char c);

/* returns a random boolean value */
bool randBool();

#endif
