#ifndef AUX_H
#define AUX_H

#include <stdint.h>
#include <stdlib.h>

#define LENGTH(x) (sizeof(x) / sizeof((x)[0]))

struct position {
	size_t x, y;
};

void die(const char *fmt, ...);
uint8_t ctoi(char c);

#endif
