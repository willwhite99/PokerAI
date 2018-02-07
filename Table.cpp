#include "Table.h"
#include "Card.h"
#include "Deck.h"
#include "Player.h"
#include "AIPlayer.h"
#include "GameOverScene.h"
#include "GameState.h"
#include "MCTS.h"
#include "Utility.h"

USING_NS_CC;

#define PADDING 100
#define INITIAL_PADDING 250
#define MAX_FOLD_COUNT 5
#define CHEN_FOLD_THRES 5.f
#define DRAW_INIT(a, b) std::pair<CardPriviledge, int>(a, b)

std::pair<CardPriviledge, int> Table::s_drawOrder[4] = { DRAW_INIT(Private, 2), DRAW_INIT(Public, 3), DRAW_INIT(Public, 1), DRAW_INIT(Public, 1) };
static cocos2d::CCAction* kActiveAction = NULL;

Table::Table(Player* human, Player* agent, cocos2d::CCLayer* parentLayer)
	: m_human(human), m_agent((AIPlayer*)agent), m_pot(0), m_stage(0), m_playerBet(0), m_aiFoldCount(0), m_playerActionRequired(false)
{
	m_deck = new Deck(parentLayer);
	m_deck->shuffle();

	m_potLabel = cocos2d::CCLabelTTF::create("Pot: 0", "Arial", 18);
	m_potLabel->setPosition(ccp(50, 300));
	parentLayer->addChild(m_potLabel);

	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

	m_humanFunds = cocos2d::CCLabelTTF::create("Player: 0", "Arial", 18);
	m_humanFunds->setPosition(ccp((visibleSize.width / 2) + 125,
		origin.y + 135));
	m_humanFunds->setAnchorPoint(ccp(0, 0.5f));
	parentLayer->addChild(m_humanFunds);
	m_humanBet = cocos2d::CCLabelTTF::create("Bet: 0", "Arial", 18);
	m_humanBet->setPosition(ccp((visibleSize.width / 2) + 125,
		origin.y + 105));
	m_humanBet->setAnchorPoint(ccp(0, 0.5f));
	parentLayer->addChild(m_humanBet);
	m_agentFunds = cocos2d::CCLabelTTF::create("Computer: 0", "Arial", 18);
	m_agentFunds->setPosition(ccp(300, visibleSize.height - 18));
	parentLayer->addChild(m_agentFunds);
	m_huamnPot = cocos2d::CCLabelTTF::create("Player Bet: 0", "Arial", 18);
	m_huamnPot->setPosition(ccp(60, 165));
	m_huamnPot->setAnchorPoint(ccp(0, 0.5f));
	parentLayer->addChild(m_huamnPot);
	m_agentPot = cocos2d::CCLabelTTF::create("AI Bet: 0", "Arial", 18);
	m_agentPot->setPosition(ccp(visibleSize.width - 100, visibleSize.height - 160));
	parentLayer->addChild(m_agentPot);
	m_agentAction = cocos2d::CCLabelTTF::create("", "Arial", 18);
	m_agentAction->setPosition(ccp(visibleSize.width / 2, 330));
	parentLayer->addChild(m_agentAction);

	updateLabels();

	nextPlay();
}

void Table::nextPlay()
{
	if (m_stage > 4)
		return;

	std::pair<CardPriviledge, int> currentDraw = s_drawOrder[m_stage++];

	if (currentDraw.first == Private)
	{
		for (int i = 0; i < currentDraw.second * 2; i++)
		{
			int selector = i % 2;
			if (selector == 0)
				m_human->addCard(m_deck->drawCard(), ccp(0,80));
			else m_agent->addCard(m_deck->drawCard(), ccp(800, 450), false, false);
		}
	}
	else
		for (int i = 0; i < currentDraw.second; i++)
			addCard(m_deck->drawCard());
}

void Table::humanWin(Score& human, Score& agent)
{
	CCLOG("Human won with a Hand:%i and Strength: %i %i", human.HandValue, human.Strength[0], human.Strength[1]);
	CCLOG("Agent lost with a Hand:%i and Strength: %i %i", agent.HandValue, agent.Strength[0], agent.Strength[1]);

	m_human->recieveFunds(m_pot);
}

void Table::agentWin(Score& human, Score& agent)
{
	CCLOG("Agent won with a Hand:%i and Strength: %i %i", agent.HandValue, agent.Strength[0], agent.Strength[1]);
	CCLOG("Human lost with a Hand:%i and Strength: %i %i", human.HandValue, human.Strength[0], human.Strength[1]);

	m_agent->recieveFunds(m_pot);
}

void Table::updateLabels()
{
	std::string base = "Pot: " + std::to_string(m_pot);
	m_potLabel->setString(base.c_str());

	base = "Player: " + std::to_string(m_human->getFunds());
	m_humanFunds->setString(base.c_str());

	base = "Computer: " + std::to_string(m_agent->getFunds());
	m_agentFunds->setString(base.c_str());

	base = "Bet: " + std::to_string(m_playerBet);
	m_humanBet->setString(base.c_str());

	base = "Player Bet: " + std::to_string(m_human->getBet());
	m_huamnPot->setString(base.c_str());

	base = "Computer Bet: " + std::to_string(m_agent->getBet());
	m_agentPot->setString(base.c_str());
}

void Table::bet(int amount)
{
	if (m_playerBet + amount < 0)
		return;

	if (m_stage != 5)
	{
		m_playerBet += amount;
		updateLabels();
	}
}

void Table::winner()
{
	Card** player = m_human->getHand();
	Card** ai = m_agent->getHand();
	std::vector<CardBase*> humanCards;
	humanCards.push_back(player[0]);
	humanCards.push_back(player[1]);
	std::vector<CardBase*> aiCards;
	aiCards.push_back(ai[0]);
	aiCards.push_back(ai[1]);
	
	std::vector<CardBase*> openCards;
	for (int i = 0; i < m_openCards.size(); i++)
		openCards.push_back(m_openCards[i]);
	//sort and detect winning hand
	Score humanScore = getScore(humanCards.data(), openCards.data(), 2, openCards.size());
	Score agentScore = getScore(aiCards.data(), openCards.data(), 2, openCards.size());

	//CCLOG("Human Hand:%f and Strength: %i %i", humanScore.Hand, humanScore.Strength[0], humanScore.Strength[1]);
	//CCLOG("Agent Hand:%f and Strength: %i %i", agentScore.Hand, agentScore.Strength[0], agentScore.Strength[1]);

	if (humanScore.Hand >= agentScore.Hand)
	{
		// could be a win for us, lets check
		if (humanScore.Hand == agentScore.Hand)
		{
			if (humanScore.Strength[0] > agentScore.Strength[0])
				humanWin(humanScore, agentScore);
			else if (humanScore.Strength[0] == agentScore.Strength[0])
				if (humanScore.Strength[1] > agentScore.Strength[1])
					humanWin(humanScore, agentScore);
				else if (humanScore.Strength[1] == agentScore.Strength[1])
					// tie
				{
					CCLOG("Was a tie!");
					m_agent->recieveFunds(m_pot / 2);
					m_human->recieveFunds(m_pot / 2);
				}
				else
					agentWin(humanScore, agentScore);
			else
				agentWin(humanScore, agentScore);
		}
		else
			humanWin(humanScore, agentScore);
	}
	else
		agentWin(humanScore, agentScore);
}

void Table::reset()
{
	// Now reset everything
	m_pot = 0;
	for (int i = 0; i < m_openCards.size(); i++)
		m_openCards[i]->getAvatar()->setVisible(false); // do not delete assets
	m_openCards.clear();

	if (m_agent->getFunds() == 0 || m_human->getFunds() == 0)
	{
		CCScene* scene = GameOverScene::scene();
		GameOverScene* gScene = dynamic_cast<GameOverScene*>(scene->getChildren()->objectAtIndex(0));
		CCAssert(gScene != NULL, "Could not find GameOverScene");

		gScene->setWinner((m_agent->getFunds() == 0));

		CCDirector::sharedDirector()->replaceScene(scene);
	}

	m_human->nextGame();
	m_agent->nextGame();
	m_human->resetBet();
	m_agent->resetBet();

	updateLabels();

	m_stage = 0;

	m_deck->shuffle(); // reshuffle

	// deal initial cards
	nextPlay();
}

void Table::fold(bool human)
{
	CCLOG("AI is folding!");
	m_pot += m_human->getBet() + m_agent->getBet();

	if (human)
		m_agent->recieveFunds(m_pot);
	else m_human->recieveFunds(m_pot);

	reset();
}

void Table::addCard(Card* card)
{
	card->setScale(0.f);
	card->setPosition(ccp(INITIAL_PADDING + PADDING*m_openCards.size(), 245));
	card->getAvatar()->setVisible(true);

	card->runAction(cocos2d::CCScaleTo::create(0.33f, 1.f));

	m_openCards.push_back(card);
}

void Table::clearActionString()
{
	m_agentAction->setString("");
	kActiveAction = NULL;
}

void Table::setActionString(const char* str)
{
	if (kActiveAction)
	{
		m_agentAction->stopAction(kActiveAction);
		kActiveAction = NULL;
	}

	m_agentAction->setString(str);
	kActiveAction = m_agentAction->runAction(CCSequence::create(
		CCDelayTime::create(5.f), 
		CCCallFunc::create(this, callfunc_selector(Table::clearActionString)), 0));
}

void Table::betting()
{
	if (!m_agent->isAllIn())
		m_human->addBet(m_playerBet);
	else m_human->addBet(m_agent->getBet() - m_human->getBet());
	m_playerBet = 0;
	bool aifold = false;

	if (m_human->getFunds() == 0)
		m_playerActionRequired = false;

	if ((!m_playerActionRequired || m_human->getBet() > m_agent->getBet()) && m_stage != 5)
	{
		std::vector<CardBase> cards;
		Card** agentCards = m_agent->getHand();
		cards.push_back(*agentCards[0]);
		cards.push_back(*agentCards[1]);

		// add comunity cards
		for (int i = 0; i < m_openCards.size(); i++)
		{
			cards.push_back(*m_openCards[i]);
		}

		GameState* currentState = new GameState(cards);
		
		srand(time(NULL));

		float value = 0.f;

		if (!aifold && m_stage != 1 && m_stage != 4)
		{
			// use MCTS for rest of game
			GameState* aimState = MCTS_BestMove(currentState);
			value = aimState->getValue();
			CCLOG("The AI is aiming for: %s with a value of: %f", aimState->getCardState().c_str(), aimState->getValue());
		}
		// Chen formula for first play
		else if (cards.size() == 2)
		{
			float score = chen(cards[0], cards[1]);
			CCLOG("First hand score: %f", score);

			int divisor = MAX_FOLD_COUNT - m_aiFoldCount;
			int bluff = rand() % divisor;
			if (score < CHEN_FOLD_THRES && bluff != 0)
			{
				// fold - bad hand
				fold(false);
				aifold = true;
				m_aiFoldCount++; // increase fold count
				m_agent->actionComplete();
				setActionString("Computer has folded.");
			}
			else if (score < CHEN_FOLD_THRES && bluff == 0)
			{
				m_agent->setIsBluffing(true);
				CCLOG("AI is bluffing ;)");
				m_aiFoldCount = 0;
			}

			value = score;
		}

		if (!aifold)
		{
			if (m_stage == 4)
			{
				std::vector<CardBase> privcards(cards.begin(), cards.begin() + 2);
				std::vector<CardBase> pubcards(cards.begin() + 2, cards.end());
				Score score = getScore(privcards.data(), pubcards.data(), 2, 5);
				Score pubscore = getScore(NULL, pubcards.data(), 0, 5);
				bool handaffect = score.HandValue != pubscore.HandValue;
				value = (score.Hand * ((handaffect)?50:1));
				CCLOG("Final value: %f", value);
			}

			BetOption option = m_agent->getBetAmount(value, m_human->getBet(), cards.size() == 2);

			switch (option.BetAction)
			{
			case BetOption::Fold:
				if (m_human->getBet() != 0)
				{
					fold(false);
					aifold = true;
					m_agent->actionComplete();
					setActionString("Computer has folded.");
					break;
				}
				else option.BetAction = BetOption::Call; // check
			case BetOption::Call:
				if (!m_agent->addBet(m_human->getBet() - m_agent->getBet()))
				{
					// go all in
					m_agent->addBet(m_agent->getFunds());
				}
				m_agent->actionComplete();
				if (m_agent->getFunds() <= 0)
					setActionString("Computer has gone all in!");
				else
					setActionString("Computer has called.");
				break;
			case BetOption::Raise:
			{
				int betamount = m_human->getBet() + option.BetAmount;
				if (m_human->isAllIn())
					betamount = m_human->getBet();
				if (!m_agent->addBet(betamount))
				{
					// go all in
					m_agent->addBet(m_agent->getFunds());
					CCLOG("All-in");
				}
				std::string c = "Computer has raised by: " + std::to_string(option.BetAmount);
				setActionString(c.c_str());
				m_agent->actionComplete();
				break;
			}
			}
		}
	}

	if (m_human->getBet() != m_agent->getBet())
		m_playerActionRequired = true;

	if (!aifold && m_agent->hasDoneAction() && (m_human->getBet() == m_agent->getBet() || (!m_playerActionRequired && (m_human->isAllIn() || m_agent->isAllIn()))))
	{
		// bets are settled we can move on
		continueBet();
		m_playerActionRequired = false;
	}
	else
		m_playerActionRequired = true;

	updateLabels();
}

void Table::continueBet()
{
	if (m_stage < 5)
	{
		m_pot += m_human->getBet() + m_agent->getBet();
			
		m_human->resetBet();
		m_agent->resetBet();

		nextPlay();

		//delete aimState;
		//m_agent->bet(m_playerBet);
		// this will change
		//if (!m_human->bet(m_playerBet))
		//{
		//	CCScene* scene = GameOverScene::scene();
		//	GameOverScene* gScene = dynamic_cast<GameOverScene*>(scene->getChildren()->objectAtIndex(0));
		//	CCAssert(gScene != NULL, "Could not find GameOverScene");

		//	gScene->setWinner(false);

		//	CCDirector::sharedDirector()->replaceScene(scene);
		//}
		
	}
	else
	{
		// reset game
		m_agent->setIsBluffing(false);

		m_human->resetBet();
		m_agent->resetBet();

		reset();
	}
	
	if(m_stage == 5)
	{
		// find winner
		winner();
		m_agent->showCards();
		m_pot = 0;
	}
	
	updateLabels();
}