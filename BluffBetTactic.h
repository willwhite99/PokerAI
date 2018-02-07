#pragma once

#include "IBetTactic.h"

class BluffBetTactic : public IBetTactic
{
public:
	BetOption getBetAmount(float value, int bet, int money);
};