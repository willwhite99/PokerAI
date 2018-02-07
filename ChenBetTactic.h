#pragma once

#include "IBetTactic.h"

class ChenBetTactic : public IBetTactic
{
public:
	BetOption getBetAmount(float value, int bet, int money);
};