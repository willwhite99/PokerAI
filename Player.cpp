#include "Player.h"
#include "Card.h"
#include <algorithm>

#define PADDING 100

bool operator==(const Score& lhs, const Score& rhs)
{
	return lhs.Hand == rhs.Hand && lhs.Strength[0] == rhs.Strength[0] && lhs.Strength[1] == rhs.Strength[1];
}

Player::Player()
	: m_money(250), m_doneAction(false), m_bet(0), m_allIn(false)
{

}

void Player::addCard(Card* card, cocos2d::CCPoint origin, bool right, bool up)
{
	cocos2d::CCPoint position;
	if (right)
		position = origin + ccp(PADDING + PADDING*m_hand.size(), 0);
	else
		position = origin - ccp(PADDING + PADDING*m_hand.size(), 0);

	if (!up)
	{
		//position = ccp(800 - position.x, 450);
		card->flip();
	}

	card->setPosition(position);
	card->getAvatar()->setVisible(true);

	m_hand.push_back(card);	
}

void Player::showCards()
{
	for (int i = 0; i < m_hand.size(); i++)
	{
		m_hand[i]->flip();
	}
}

bool Player::bet(int amount)
{
	m_doneAction = true;
	if (m_money < amount)
		return false;

	m_money -= amount;

	if (m_money == 0)
		m_allIn = true;
	return true;
}

void Player::nextGame()
{
	// clear the cards for next game
	for (int i = 0; i < m_hand.size(); i++)
	{
		m_hand[i]->getAvatar()->setVisible(false);
		(m_hand[i]->isFlipped()) ? 0 : m_hand[i]->flip();
	}

	m_allIn = false;

	m_hand.clear();
}