#pragma once

#include "IBetTactic.h"

class NormalBetTactic : public IBetTactic
{
public:
	BetOption getBetAmount(float value, int bet, int money);
};