#ifndef RULES_H
#define RULES_H

#include "cell.h"

#include <stdint.h>

typedef struct {
	uint16_t born     : 9;
	uint16_t survives : 9;
} Rule;

typedef struct Variation {
	Rule              rule;
	const char       *name;
	struct Variation *next;
} Variation;

bool       shouldLive(Rule r, uint8_t neighbors, bool active);
Variation *newVariation();
void       freeVariation(Variation *n);

#endif
