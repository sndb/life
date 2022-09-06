#include "rules.h"

#include "aux.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

const char *rulesData[] = {
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
const int        nRules      = sizeof(rulesData) / sizeof(*rulesData);
static const int ruleStopper = -1;

static Rule
parseRule(const char *r) {
	const int maxLen   = sizeof("B012345678/S012345678") / sizeof(char);
	const int nLen     = 10;
	int      *born     = malloc(nLen * sizeof(int));
	int      *survives = malloc(nLen * sizeof(int));

	int state = 0, j = 0, k = 0;
	for (int i = 0; i < maxLen; i++) {
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
shouldLive(Rule r, int neighbors, bool active) {
	if (active) {
		for (int i = 0; r.survives[i] != ruleStopper; i++)
			if (neighbors == r.survives[i])
				return true;
	} else {
		for (int i = 0; r.born[i] != ruleStopper; i++)
			if (neighbors == r.born[i])
				return true;
	}
	return false;
}

static void
freeRule(Rule r) {
	free(r.born);
	free(r.survives);
}

char *
ruleName(RuleName r) {
	switch (r) {
	case Replicator:
		return "Replicator";
	case Seeds:
		return "Seeds";
	case LifeWithoutDeath:
		return "Life without Death";
	case Life:
		return "Life";
	case ThirtyFourLife:
		return "34 Life";
	case Diamoeba:
		return "Diamoeba";
	case TwoXTwo:
		return "2x2";
	case HighLife:
		return "HighLife";
	case DayAndNight:
		return "Day & Night";
	case Morley:
		return "Morley";
	case Anneal:
		return "Anneal";
	}
	return "Unknown";
}

RuleSet *
newRuleSet() {
	Rule    *rules = malloc(nRules * sizeof(Rule));
	RuleSet *s     = malloc(sizeof(RuleSet));
	for (int i = 0; i < nRules; i++)
		rules[i] = parseRule(rulesData[i]);
	s->rules  = rules;
	s->choice = Life;
	return s;
}

void
freeRuleSet(RuleSet *s) {
	for (int i = 0; i < nRules; i++)
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
getRule(RuleSet *s) {
	return s->rules[s->choice];
}

RuleName
getRuleName(RuleSet *s) {
	return s->choice;
}
