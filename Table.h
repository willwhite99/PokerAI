#pragma once

#include <vector>
#include <cocos2d.h>
#include <map>

class Player;
class AIPlayer;
class Card;
class Deck;
struct Score;

enum CardPriviledge
{
	Public,
	Private
};

class Table : public cocos2d::CCObject
{
public:
	Table(Player* human, Player* agent, cocos2d::CCLayer* parentLayer);
	void addCard(Card* card);
	void nextPlay();
	void bet(int amount);
	void fold(bool human);
	void betting();
	void continueBet();
private:
	void humanWin(Score& human, Score& agent);
	void agentWin(Score& human, Score& agent);
	void winner();

	void clearActionString();
	void setActionString(const char* str);

	void updateLabels();
	void reset();

	std::vector<Card*> m_openCards;
	Player* m_human;
	AIPlayer* m_agent;

	Deck* m_deck;

	cocos2d::CCLabelTTF* m_potLabel;
	cocos2d::CCLabelTTF* m_humanFunds;
	cocos2d::CCLabelTTF* m_humanBet;
	cocos2d::CCLabelTTF* m_huamnPot;
	cocos2d::CCLabelTTF* m_agentFunds;
	cocos2d::CCLabelTTF* m_agentPot;
	cocos2d::CCLabelTTF* m_agentAction;

	int m_pot;
	int m_stage;
	int m_playerBet;
	int m_aiFoldCount;
	bool m_playerActionRequired;
	static std::pair<CardPriviledge, int> s_drawOrder[4];
};