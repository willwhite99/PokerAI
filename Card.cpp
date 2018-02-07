#include "Card.h"
#include <assert.h>

Card::Card(const char* asset, Suit suit, int val)
	: CardBase(suit, val), CCNode(), m_up(true)
{
	m_old = 0;
	m_sprite = cocos2d::CCSprite::create(asset);
	assert(m_sprite != NULL);
	m_sprite->setScale(0.15f);
	this->addChild(m_sprite);
}

void Card::flip()
{
	m_up = !m_up;
	if (!m_up)
	{
		m_old = m_sprite->getTexture();
		cocos2d::CCTexture2D* tex = cocos2d::CCTextureCache::sharedTextureCache()->textureForKey("./cards/card_back.png");
		m_sprite->setTexture(tex);
	}
	else
		m_sprite->setTexture(m_old);
}