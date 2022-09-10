#include "rules.h"

#include "aux.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

static const char *ruleStrings[] = {
	"Replicator",
	"Seeds",
	"Life without Death",
	"Life",
	"34 Life",
	"Diamoeba",
	"2x2",
	"HighLife",
	"Day & Night",
	"Morley",
	"Anneal",
};
static const char *rulesData[] = {
	"B1357/S1357",
	"B2/S",
	"B3/S012345678",
	"B3/S23",
	"B34/S34",
	"B35678/S5678",
	"B36/S125",
	"B36/S23",
	"B3678/S34678",
	"B368/S245",
	"B4678/S35678",
};
static const size_t nRules      = sizeof(rulesData) / sizeof(*rulesData);
static const int8_t ruleStopper = -1;

static Rule
parseRule(const char *r) {
	const size_t maxLen   = sizeof("B012345678/S012345678") / sizeof(char);
	const size_t nLen     = 10;
	int8_t      *born     = malloc(nLen * sizeof(int8_t));
	int8_t      *survives = malloc(nLen * sizeof(int8_t));

	uint8_t state = 0;
	size_t  j = 0, k = 0;
	for (size_t i = 0; i < maxLen; i++) {
		const char c = r[i];
		switch (state) {
		case 0: /* B */
			assert(c == 'B');
			state++;
			break;
		case 1: /* number after B */
			if (isdigit(c)) {
				born[j] = ctoi(c);
				j++;
			} else {
				born[j] = ruleStopper;
				i--;
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
				survives[k] = ctoi(c);
				k++;
			} else {
				survives[k] = ruleStopper;
				goto out;
			}
			break;
		default:
			fprintf(stderr, "unknown state %d", state);
			exit(1);
		}
	}
out:
	return (Rule){.born = born, .survives = survives};
}

bool
shouldLive(Rule r, uint8_t neighbors, bool active) {
	if (active) {
		for (; *r.survives != ruleStopper; r.survives++)
			if (neighbors == *r.survives)
				return true;
	} else {
		for (; *r.born != ruleStopper; r.born++)
			if (neighbors == *r.born)
				return true;
	}
	return false;
}

static void
freeRule(Rule r) {
	free(r.born);
	free(r.survives);
}

const char *
ruleString(RuleName r) {
	return ruleStrings[r];
}

RuleSet *
newRuleSet() {
	Rule    *rules = malloc(nRules * sizeof(Rule));
	RuleSet *s     = malloc(sizeof(RuleSet));
	for (size_t i = 0; i < nRules; i++)
		rules[i] = parseRule(rulesData[i]);
	s->rules  = rules;
	s->choice = Life;
	return s;
}

void
freeRuleSet(RuleSet *s) {
	for (size_t i = 0; i < nRules; i++)
		freeRule(s->rules[i]);
	free(s->rules);
	free(s);
}

Rule
changeRule(RuleSet *s) {
	s->choice = (s->choice + 1) % nRules;
	return s->rules[s->choice];
}

Rule
getRule(const RuleSet *s) {
	return s->rules[s->choice];
}

RuleName
getRuleName(const RuleSet *s) {
	return s->choice;
}
