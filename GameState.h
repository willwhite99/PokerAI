#pragma once

#include <vector>
#include "Player.h"

class Player;
class CardBase;

class GameState
{
public:
	GameState(std::vector<CardBase> cards) :
		m_visibleCards(cards), m_isTerminal(false) { m_isTerminal = cards.size() == 7;	}
	GameState(GameState* copy);
	~GameState();
	//std::vector<Player*> getPlayers() const { return m_players; }
	std::vector<CardBase> getCards() const { return m_visibleCards; }
	bool isTerminalState() const { return m_isTerminal; }
	int getRemainingCardCount() const { return m_remainingCards.size(); }
	std::vector<CardBase> availableCards();
	void addCardToGame(CardBase card);
	float getValue() const { return m_value; }
	void setValue(const float val) { m_value = val; }
	std::string getCardState();
	CardBase getRandCard(bool removeFromVec = false);
	CardBase getNextCard(bool removeFromVec = false);
private:
	//std::vector<Player*> m_players;
	// assume the first 2 cards are the private cards (in case for texas holdem)
	std::vector<CardBase> m_visibleCards;
	std::vector<CardBase> m_remainingCards;
	float m_value;
	bool m_isTerminal;
};

template<typename T>
struct pointer_equals
{
	const T* find;

	bool operator()(const T* other) const
	{
		return *find == *other;
	}
};

Score getScore(CardBase** privateCards, CardBase** publicCards, int privateCount, int publicCount);
Score getScore(CardBase* privateCards, CardBase* publicCards, int privateCount, int publicCount);
Score getScore(std::vector<CardBase*> privateCards, std::vector<CardBase*> publicCards, int privateCount, int publicCount);
int getHandVariance(std::vector<CardBase>& cards, int start, Score target);
float outToPercentage(int out, bool turn);
void setScore(Score& score, int handvalue);
std::vector<int>::iterator findFirstFromBack(std::vector<int>& ranks, std::vector<int>::iterator it, int test, int distTraveled);