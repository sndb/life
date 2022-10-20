#include "field.h"
#include "preprocess.h"

static bool
inRange(const struct field *f, size_t x, size_t y)
{
	return x < f->x && y < f->y;
}

static uint8_t
neighbors(const struct field *f, size_t x, size_t y)
{
	uint8_t sum = 0;
	for (int8_t dx = -1; dx <= 1; dx++) {
		for (int8_t dy = -1; dy <= 1; dy++) {
			if (dx == 0 && dy == 0)
				continue;
			const size_t nx = x + dx;
			const size_t ny = y + dy;
			if (inRange(f, nx, ny) && alive(f->matrix[xytoi(f, nx, ny)]))
				sum++;
		}
	}
	return sum;
}

void
fieldPreprocess(struct field *f)
{
	for (size_t i = 0; i < f->x; i++)
		for (size_t j = 0; j < f->y; j++) {
			const size_t pos = xytoi(f, i, j);
			f->matrix[pos] = (f->matrix[pos] % ~(0xF << 1)) | (neighbors(f, i, j) << 1);
		}
}
