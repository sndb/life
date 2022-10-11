#ifndef RULES_H
#define RULES_H

#include "cell.h"

#include <assert.h>
#include <stdint.h>

typedef struct {
	uint16_t born     : 9;
	uint16_t survives : 9;
} Rule;

static_assert(sizeof(Rule) == 4, "Rule size is 4 bytes");

typedef struct Variation {
	struct Variation *next;
	const char       *name;
	Rule              rule;
} Variation;

static_assert(sizeof(Variation) == 24, "Variation size is 24 bytes");

bool       shouldLive(Rule r, uint8_t neighbors, bool active);
Variation *newVariation();
void       freeVariation(Variation *n);

#endif
