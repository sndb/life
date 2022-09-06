#ifndef RULES_H
#define RULES_H

#include "cell.h"

typedef enum {
	Replicator,
	Seeds,
	LifeWithoutDeath,
	Life,
	ThirtyFourLife,
	Diamoeba,
	TwoXTwo,
	HighLife,
	DayAndNight,
	Morley,
	Anneal,
} RuleName;

typedef struct {
	int *born;
	int *survives;
} Rule;

typedef struct {
	Rule    *rules;
	RuleName choice;
} RuleSet;

bool     shouldLive(Rule r, int neighbors, bool active);
char    *ruleName(RuleName r);
RuleSet *newRuleSet();
void     freeRuleSet(RuleSet *s);
Rule     changeRule(RuleSet *s);
Rule     getRule(RuleSet *s);
RuleName getRuleName(RuleSet *s);

#endif
