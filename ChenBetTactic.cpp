#include "ChenBetTactic.h"

#define FOLD_THRES 5.f
#define CALL_THRES 8.f

#define FOLD_DIVIDER 3.5f
#define RAISE_LIMIT 70

BetOption ChenBetTactic::getBetAmount(float value, int bet, int money)
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

		int valuei = (int)value;
		int r = valuei % 10;
		
		if (bet < RAISE_LIMIT)
			option.BetAmount = (valuei / 10) + (r != 0) ? 10 : 0;
		else
			option.BetAction = BetOption::Call;
	}

	return option;
}