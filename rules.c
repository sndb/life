#include "rules.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

static const struct variation variations[] = {
	{"Life",		"B3/S23"},
	{"Replicator",		"B1357/S1357"},
	{"Seeds",		"B2/S"},
	{"Life without Death",	"B3/S012345678"},
	{"34 Life",		"B34/S34"},
	{"Diamoeba",		"B35678/S5678"},
	{"2x2",			"B36/S125"},
	{"HighLife",		"B36/S23"},
	{"Day & Night",		"B3678/S34678"},
	{"Morley",		"B368/S245"},
	{"Anneal",		"B4678/S35678"},
};

struct variation
getVariation(void)
{
	static size_t i = 0;
	i %= sizeof(variations) / sizeof(variations[0]);
	return variations[i++];
}

static uint8_t
ctoi(char c)
{
	assert(c >= '0' && c <= '9');
	return c - '0';
}

static void
expect_b(const char **s)
{
	assert(**s == 'B');
	(*s)++;
}

static void
expect_slash(const char **s)
{
	assert(**s == '/');
	(*s)++;
}

static void
expect_s(const char **s)
{
	assert(**s == 'S');
	(*s)++;
}

static uint16_t
accept_number(const char **s)
{
	uint16_t n = 0;
	while (isdigit(**s)) {
		const uint8_t i = ctoi(**s);
		assert(i <= 8);
		n |= 1 << i;
		(*s)++;
	}
	return n;
}

uint32_t
parseCode(const char *s)
{
	expect_b(&s);
	uint16_t born = accept_number(&s);
	expect_slash(&s);
	expect_s(&s);
	uint16_t survives = accept_number(&s);
	return born << 9 | survives;
}

bool
shouldLive(uint32_t rule, uint8_t cell)
{
	if (cell & 1)
		return rule & 1 << (cell >> 1);
	else
		return rule & 1 << 9 << (cell >> 1);
}
