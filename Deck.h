#pragma once

#include <cocos2d.h>

class Card;

class Deck
{
public:
	Deck(cocos2d::CCLayer* scene);
	~Deck();

	void shuffle();
	Card* drawCard();
private:
	Card** m_deck;
	int m_deckCounter;
};