#include "field.h"

#include "rules.h"

#include <stdbool.h>
#include <stdlib.h>

size_t
xytoi(const struct field *f, size_t x, size_t y)
{
	return x + y * f->x;
}

bool
alive(uint8_t cell)
{
	return cell & 1;
}

struct field *
fieldCreate(size_t x, size_t y)
{
	struct field *f = calloc(1, sizeof(*f) + x * y * sizeof(*f->matrix));
	f->x = x;
	f->y = y;
	return f;
}

void
fieldCompile(struct field *f, uint32_t rule)
{
	for (size_t i = 0; i < f->x; i++) {
		for (size_t j = 0; j < f->y; j++) {
			const size_t k = xytoi(f, i, j);
			f->matrix[k] = shouldLive(rule, f->matrix[k]);
		}
	}
}

void
fieldPermutate(struct field *f, size_t n)
{
	for (size_t i = 0; i < n; i++)
		f->matrix[rand() % (f->x * f->y)] = 1;
}

void
fieldRandomize(struct field *f)
{
	fieldPermutate(f, f->x * f->y);
}

void
fieldClear(struct field *f)
{
	for (size_t i = 0; i < f->x * f->y; i++)
		f->matrix[i] = 0;
}
