#ifndef FIELD_H
#define FIELD_H

#include "rules.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

struct field {
	size_t x, y;
	uint8_t matrix[];
};

static_assert(sizeof(struct field) == 16, "16 byte field size");

size_t matrixPos(const struct field *f, size_t x, size_t y);
bool cellAlive(uint8_t cell);
struct field *fieldCreate(size_t x, size_t y);
void fieldCompile(struct field *f, uint32_t rule);
void fieldPermutate(struct field *f, size_t n);
void fieldRandomize(struct field *f);
void fieldClear(struct field *f);

#endif
