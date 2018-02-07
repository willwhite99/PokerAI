#pragma once

#include <cocos2d.h>

enum Suit
{
	Clubs = 0,
	Spades,
	Diamonds,
	Hearts
};

class CardBase
{
public:
	CardBase(Suit suit, int val) : m_suit(suit), m_value(val) { }
	int getValue() const { return m_value; }
	Suit getSuit() const { return m_suit; }
	bool operator==(const CardBase& rhs) const { return (m_suit == rhs.m_suit) && (m_value == rhs.m_value); }
protected:
	Suit m_suit;
	int m_value;
};

class Card : public CardBase, public cocos2d::CCNode
{
public:
	Card(const char* asset, Suit suit, int val);
	cocos2d::CCSprite* getAvatar() const { return m_sprite; }
	void flip();
	bool isFlipped() const { return m_up; }
private:
	cocos2d::CCSprite* m_sprite;
	cocos2d::CCTexture2D* m_old;
	// facing up?
	bool m_up;
};