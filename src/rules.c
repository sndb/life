#include "rules.h"

#include "aux.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

static const char *variationNames[] = {
	"Life",
	"Replicator",
	"Seeds",
	"Life without Death",
	"34 Life",
	"Diamoeba",
	"2x2",
	"HighLife",
	"Day & Night",
	"Morley",
	"Anneal",
};
static const char *variationsData[] = {
	"B3/S23",
	"B1357/S1357",
	"B2/S",
	"B3/S012345678",
	"B34/S34",
	"B35678/S5678",
	"B36/S125",
	"B36/S23",
	"B3678/S34678",
	"B368/S245",
	"B4678/S35678",
};
static const size_t nVariations = sizeof(variationsData) / sizeof(*variationsData);

static Rule
parseRule(const char *s) {
	Rule    rule  = {.born = 0, .survives = 0};
	uint8_t state = 0;
	for (char c; (c = *s); s++) {
		switch (state) {
		case 0: /* B */
			assert(c == 'B');
			state++;
			break;
		case 1: /* number after B */
			if (isdigit(c)) {
				const uint8_t n = ctoi(c);
				assert(n <= 8);
				rule.born |= 1 << n;
			} else {
				s--;
				state++;
			}
			break;
		case 2: /* / */
			assert(c == '/');
			state++;
			break;
		case 3: /* S */
			assert(c == 'S');
			state++;
			break;
		case 4: /* number after S */
			if (isdigit(c)) {
				const uint8_t n = ctoi(c);
				assert(n <= 8);
				rule.survives |= 1 << n;
			} else {
				die("unknown character %c after the second number\n", c);
			}
			break;
		default:
			die("bad state %d\n", state);
		}
	}
	return rule;
}

bool
shouldLive(Rule r, uint8_t neighbors, bool active) {
	return active ? (r.survives & 1 << neighbors) > 0 : (r.born & 1 << neighbors) > 0;
}

Variation *
newVariation() {
	Variation *head = NULL;
	Variation *tail = NULL;
	for (size_t i = 0; i < nVariations; i++) {
		Variation *curr = malloc(sizeof(Variation));
		if (!head)
			head = curr;
		curr->rule = parseRule(variationsData[i]);
		curr->name = variationNames[i];
		if (tail)
			tail->next = curr;
		tail = curr;
	}
	tail->next = head;
	return head;
}

void
freeVariation(Variation *n) {
	if (n == n->next) {
		free(n);
		return;
	}
	Variation *p = n;
	while (p->next != n)
		p = p->next;
	p->next = p->next->next;
	freeVariation(n->next);
	free(n);
}
