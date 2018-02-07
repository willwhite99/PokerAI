#include "NormalBetTactic.h"

#define FOLD_THRES 40.f
#define CALL_THRES 90.f

#define FOLD_DIVIDER 5.f
#define RAISE_LIMIT 160

BetOption NormalBetTactic::getBetAmount(float value, int bet, int money)
{
	BetOption option;
	option.BetAmount = 0;

	if (value < FOLD_THRES || (value < CALL_THRES && bet / FOLD_DIVIDER > money))
		option.BetAction = BetOption::Fold;
	else if (value < CALL_THRES)
		option.BetAction = BetOption::Call;
	else
	{
		// raise
		option.BetAction = BetOption::Raise;

		int valuei = (int)(value / 3) / 10;
		valuei *= 10;

		if (bet < RAISE_LIMIT)
			option.BetAmount = (valuei > RAISE_LIMIT) ? RAISE_LIMIT : valuei;
		else option.BetAction = BetOption::Call;
	}

	return option;
}