#pragma once

#include "Player.h"
#include "IBetTactic.h"
#include <map>

enum Tactics
{
	Bluffing,
	Normal,
	Chen
};

class AIPlayer : public Player
{
public:
	AIPlayer();
	// get action depending on hand value, and current bet
	BetOption getBetAmount(float value, int bet, bool chen);
	void setIsBluffing(const bool val) { m_bluffing = val; }
private:
	bool m_bluffing;
	static std::map<Tactics, IBetTactic*> m_tactics;
};