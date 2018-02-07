#include "AIPlayer.h"
#include "BluffBetTactic.h"
#include "NormalBetTactic.h"
#include "ChenBetTactic.h"

#define TACTIC_INIT(a, b) std::pair<Tactics, IBetTactic*>(a, b)
std::map<Tactics, IBetTactic*> AIPlayer::m_tactics = { TACTIC_INIT(Bluffing, new BluffBetTactic()), TACTIC_INIT(Normal, new NormalBetTactic()), TACTIC_INIT(Chen, new ChenBetTactic()) };

AIPlayer::AIPlayer()
	: Player(), m_bluffing(false)
{
}

BetOption AIPlayer::getBetAmount(float value, int bet, bool chen)
{
	BetOption option;

	if (chen)
		option = m_tactics[Chen]->getBetAmount(value, bet, m_money);
	else if (m_bluffing)
		option = m_tactics[Bluffing]->getBetAmount(value, bet, m_money);
	else option = m_tactics[Normal]->getBetAmount(value, bet, m_money);

	if (m_allIn && m_money == 0)
	{
		option.BetAmount = 0;
		option.BetAction = BetOption::Call;
	}

	return option;
}