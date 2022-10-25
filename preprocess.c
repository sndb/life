#include "preprocess.h"

#include "field.h"

static bool
check(const struct field *f, size_t i)
{
	return i < f->x * f->y && alive(f->matrix[i]);
}

static uint8_t
neighbors(const struct field *f, size_t i)
{
	return check(f, i - 1) + check(f, i + 1)
	     + check(f, i - f->x) + check(f, i + f->x)
	     + check(f, i - f->x - 1) + check(f, i + f->x - 1)
	     + check(f, i - f->x + 1) + check(f, i + f->x + 1);
}

void
fieldPreprocess(struct field *f)
{
	for (size_t i = 0; i < f->x * f->y; i++)
		f->matrix[i] = f->matrix[i] % ~(0xF << 1) | neighbors(f, i) << 1;
}
