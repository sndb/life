#ifndef RULES_H
#define RULES_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

struct variation {
	const char *name;
	const char *code;
};

struct variation getVariation(void);
uint32_t parseCode(const char *s);
bool shouldLive(uint32_t, uint8_t cell);

#endif
