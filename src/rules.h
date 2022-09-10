#ifndef RULES_H
#define RULES_H

#include "cell.h"

#include <stdint.h>

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
	int8_t *born;
	int8_t *survives;
} Rule;

typedef struct {
	Rule    *rules;
	RuleName choice;
} RuleSet;

bool        shouldLive(Rule r, uint8_t neighbors, bool active);
const char *ruleString(RuleName r);
RuleSet    *newRuleSet();
void        freeRuleSet(RuleSet *s);
Rule        changeRule(RuleSet *s);
Rule        getRule(const RuleSet *s);
RuleName    getRuleName(const RuleSet *s);

#endif
