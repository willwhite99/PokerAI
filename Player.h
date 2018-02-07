#pragma once

#include <vector>
#include "cocos2d.h"

class Card;
class CardBase;

struct Score
{
	// value of Pair, 3 of kind, straight, flush etc.
	float Hand;
	// used for identification
	unsigned int HandValue;
	// King high? we may have to check for 2 values
	unsigned int Strength[2];
};

extern bool operator==(const Score& lhs, const Score& rhs);

class Player
{
public:
	Player();
	
	void addCard(Card* card, cocos2d::CCPoint origin, bool right = true, bool up = true);
	void showCards();
	bool bet(int amount);
	int getFunds() const { return m_money; }
	void recieveFunds(int amount) { m_money += amount; (m_money > 0) ? m_allIn = false : m_allIn = true; }
	Card** getHand() { return m_hand.data(); }
	void nextGame();
	bool addBet(const int b) { bool be = bet(b);  if (be){ m_bet += b; } return be; }
	void resetBet() { m_bet = 0; }
	int getBet() const { return m_bet; }
	bool hasDoneAction() const { return m_doneAction; }
	void resetAction() { m_doneAction = false; }
	void actionComplete() { m_doneAction = true; }
	void setMoney(int val) { m_money = val; }

	bool isAllIn() const { return m_allIn; }
protected:
	int m_money;
	int m_bet;
	bool m_doneAction;
	bool m_allIn;
private:
	std::vector<Card*> m_hand;
};