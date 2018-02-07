#include "BluffBetTactic.h"
#include <cstdlib>

#define BLUFF_RAND 4

#define CALL_THRES 50.f

#define FOLD_DIVIDER 3.33f

BetOption BluffBetTactic::getBetAmount(float value, int bet, int money)
{
	BetOption option;
	option.BetAmount = 0;

	// sometimes do something crazy
	bool random = (rand() % BLUFF_RAND) == 0;
	bool raise = (rand() % 2) == 0;

	if ((bet / FOLD_DIVIDER) * (random) ? 2 : 1 > money)
		option.BetAction = BetOption::Fold;
	else if (value < CALL_THRES && !raise)
		option.BetAction = BetOption::Call;
	else
	{
		// raise
		option.BetAction = BetOption::Raise;
		// amount to raise by
		option.BetAmount = (raise) ? 30 : 20;
		if (random)
			option.BetAmount *= 4;// really crazy
	}

	return option;
}