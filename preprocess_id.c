#include "field.h"
#include "preprocess.h"

static bool
inRange(const struct field *f, size_t i)
{
	return i < f->x * f->y;
}

static bool
occupied(const struct field *f, size_t i)
{
	return inRange(f, i) && cellAlive(f->matrix[i]);
}

static uint8_t
neighbors(const struct field *f, size_t i)
{
	return occupied(f, i - 1) + occupied(f, i + 1)
	     + occupied(f, i - f->x) + occupied(f, i + f->x)
	     + occupied(f, i - f->x - 1) + occupied(f, i + f->x - 1)
	     + occupied(f, i - f->x + 1) + occupied(f, i + f->x + 1);
}

void
fieldPreprocess(struct field *f)
{
	for (size_t i = 0; i < f->x * f->y; i++)
		f->matrix[i] = f->matrix[i] % ~(0xF << 1) | neighbors(f, i) << 1;
}
