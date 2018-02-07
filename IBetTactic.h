#pragma once

struct BetOption
{
	unsigned int BetAmount;
	enum Action
	{
		Fold,
		Call,
		Raise
	} BetAction;
};

class IBetTactic
{
public:
	virtual BetOption getBetAmount(float value, int bet, int money) = 0;
};