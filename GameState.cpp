#include "GameState.h"
#include "Card.h"
#include "Player.h"
#include <algorithm>

// weighting for combinations
#define PAIR_VALUE 1.f
#define TWOPAIR_VALUE 1.9f
#define THREEKIND_VALUE 2.25f
#define STRAIGHT_VALUE 2.5f
#define FLUSH_VALUE 3.f
#define FULL_VALUE 4.f
#define FOUR_VALUE 5.f
#define RUN_VALUE 6.f

// outs to percentage
#define FLOP_TO_TURN_PERC 2.13333f
#define TURN_TO_RIVER_PERC 2.16f

GameState::GameState(GameState* copy)
{
	m_visibleCards = std::vector<CardBase>(copy->m_visibleCards);
	if (m_visibleCards.size() >= 7)
		m_isTerminal = true;
	else m_isTerminal = false;
	availableCards();
}

std::vector<CardBase> GameState::availableCards()
{
	// delete any cards in already
	//for (std::vector<CardBase>::iterator it = m_remainingCards.begin(); it != m_remainingCards.end(); it++)
	//{
	//	delete *it;
	//}

	m_remainingCards.clear();

	// only account for our cards
	for (int suit = 0; suit < 4; suit++)
	{
		for (int j = 1; j <= 13; j++)
		{
			CardBase card = CardBase((Suit)suit, j);
			//pointer_equals<CardBase> eq = { card };
			// must do a pointer ==
			if (std::find(m_visibleCards.begin(), m_visibleCards.end(), card) == m_visibleCards.end())
			{
				m_remainingCards.push_back(card);
			}
		}
	}

	return m_remainingCards;
}

void GameState::addCardToGame(CardBase card)
{
	m_visibleCards.push_back(card);

	// after we have 7 cards we are at the leaf nodes
	if (m_visibleCards.size() >= 7)
		m_isTerminal = true;

	//availableCards();
}

CardBase GameState::getRandCard(bool removeFromVec)
{
	int randIndex = (m_remainingCards.size() != 1) ? rand() % (m_remainingCards.size() - 1) : 0;
	CardBase retVal = m_remainingCards[randIndex];

	if (removeFromVec)
	{
		//pointer_equals<CardBase> eq = { retVal };
		m_remainingCards.erase(std::find(m_remainingCards.begin(), m_remainingCards.end(), retVal));
	}

	return retVal;
}

CardBase GameState::getNextCard(bool removeFromVec)
{
	CardBase retVal = m_remainingCards.front();

	if (removeFromVec)
	{
		//pointer_equals<CardBase> eq = { retVal };
		m_remainingCards.erase(std::find(m_remainingCards.begin(), m_remainingCards.end(), retVal));
	}

	return retVal;
}

std::string GameState::getCardState()
{
	if (m_visibleCards.size() < 7)
		return "(ERROR - Not enough cards)" + std::to_string(m_visibleCards.size());
	// first 2 cards are private - in case of texas holdem
	std::vector<CardBase> privateCards;
	privateCards.push_back(m_visibleCards[0]);
	privateCards.push_back(m_visibleCards[1]);
	// rest are community cards
	std::vector<CardBase> publicCards(m_visibleCards.begin() + 2, m_visibleCards.end());
	Score score = getScore(privateCards.data(), publicCards.data(), 2, 5);

	switch (score.HandValue)
	{
	case 0:
		return "None";
	case 1:
		return "Pair";
	case 2:
		return "2 Pair";
	case 3:
		return "Three of a Kind";
	case 4:
		return "Straight";
	case 5:
		return "Flush";
	case 6:
		return "Full House";
	case 7:
		return "Four of a Kind";
	case 8:
		return "Straight Flush";
	default:
		return "None";
	}

	assert(1 == 2); // we shouldnt hit this
}

GameState::~GameState()
{
	//for (int i = 0; i < m_visibleCards.size() > 0; i++)
	//{
	//	delete m_visibleCards[i];
	//}
}

Score getScore(CardBase* privateCards, CardBase* publicCards, int privateCount, int publicCount)
{
	std::vector<CardBase*> vecPrivate, vecPublic;

	for (int i = 0; i < privateCount; i++)
		vecPrivate.push_back(&privateCards[i]);
	for (int i = 0; i < publicCount; i++)
		vecPublic.push_back(&publicCards[i]);

	return getScore(vecPrivate, vecPublic, privateCount, publicCount);
}

Score getScore(CardBase** privateCards, CardBase** publicCards, int privateCount, int publicCount)
{
	std::vector<CardBase*> vecPrivate, vecPublic;

	for (int i = 0; i < privateCount; i++)
		vecPrivate.push_back(privateCards[i]);
	for (int i = 0; i < publicCount; i++)
		vecPublic.push_back(publicCards[i]);

	return getScore(vecPrivate, vecPublic, privateCount, publicCount);
}

std::vector<int>::iterator findFirstFromBack(std::vector<int>& ranks, std::vector<int>::iterator it, int test, int distTraveled)
{
	for (int i = 0; i < ranks.size() - distTraveled; i++)
	{
		std::vector<int>::iterator rank = it - i;

		if (*rank == test)
			return rank;
	}

	return ranks.end();
}

Score getScore(std::vector<CardBase*> privateCards, std::vector<CardBase*> publicCards, int privateCount, int publicCount)
{
	// copy our hand
	std::vector<CardBase*> handCopy;
	// what is our hand??
	Score ourScore;
	ourScore.Hand = 0.f;
	ourScore.HandValue = 0;
	ourScore.Strength[0] = 0;
	ourScore.Strength[1] = 0;

	for (int i = 0; i < privateCount; i++)
		handCopy.push_back(privateCards[i]);

	for (int i = 0; i < publicCount; i++)
		handCopy.push_back(publicCards[i]);
	// sort ascending 0,1,2,3,4
	std::sort(handCopy.begin(), handCopy.end(), [](const CardBase* a, const CardBase* b)
	{
		return a->getValue() < b->getValue();
	});

	// get prior information
	std::vector<int> countByRank;
	countByRank.resize(14); // additional one for ace's higher value
	for (int i = 0; i < handCopy.size(); i++)
		countByRank[handCopy[i]->getValue()] += 1;
	std::vector<int> countBySuit;
	countBySuit.resize(4);
	for (int i = 0; i < handCopy.size(); i++)
		countBySuit[handCopy[i]->getSuit()] += 1;

	std::vector<int>::iterator pairPos = std::find(countByRank.begin(), countByRank.end(), 2);
	if (pairPos != countByRank.end())
	{
		pairPos = findFirstFromBack(countByRank, countByRank.begin() + countByRank.size() - 1, 2, 0);
		int index = std::distance(countByRank.begin(), pairPos);
		// even without a 2 pair we need to set this
		ourScore.Strength[0] = index;
		// check for 2 pairs
		//std::vector<int>::iterator secondPair = std::find(pairPos + 1, countByRank.end(), 2);
		std::vector<int>::iterator secondPair = findFirstFromBack(countByRank, pairPos - 1, 2, std::distance(pairPos, countByRank.end()));
		if (secondPair != countByRank.end())
		{
			int secondIndex = std::distance(countByRank.begin(), secondPair);
			setScore(ourScore, 2);
			ourScore.Strength[1] = secondIndex;

			// swap our strengths if 0 < 1
			if (ourScore.Strength[0] < ourScore.Strength[1])
			{
				int temp = ourScore.Strength[0];
				ourScore.Strength[0] = ourScore.Strength[1];
				ourScore.Strength[1] = temp;
			}
		}
		else
			setScore(ourScore, 1);
	}

	std::vector<int>::iterator threePos = std::find(countByRank.begin(), countByRank.end(), 3);
	if (threePos != countByRank.end())
	{
		int index = std::distance(countByRank.begin(), threePos);

		std::vector<int>::iterator secondThreePos = std::find(threePos + 1, countByRank.end(), 3);
		// do we already have a pair? this could be a full house
		if (ourScore.Hand == 1 || ourScore.Hand == 2 || secondThreePos != countByRank.end())
		{
			setScore(ourScore, 6);
			// swap relevance, three kind must be checked first
			int temp = ourScore.Strength[0];
			ourScore.Strength[0] = index;
			ourScore.Strength[1] = temp;

			// if we have 2 three of a kinds
			if (secondThreePos != countByRank.end())
			{
				// which three of a kind is better?
				int index2 = std::distance(countByRank.begin(), secondThreePos);

				bool firstbetter = index > index2;
				ourScore.Strength[0] = (firstbetter) ? index : index2;
				ourScore.Strength[1] = (firstbetter) ? index2 : index;
			}
		}
		else
		{
			setScore(ourScore, 3);
			ourScore.Strength[0] = index;
		}
	}

	// check for a straight
	// using open brackets for local variables
	{
		int counter = 0, val = handCopy[0]->getValue();
		for (int i = 0; i < handCopy.size() - 1; i++)
		{
			if (handCopy[i]->getValue() == handCopy[i + 1]->getValue())
				continue;

			if (handCopy[i]->getValue() == handCopy[i + 1]->getValue() + 1 ||
				handCopy[i]->getValue() == handCopy[i + 1]->getValue() - 1)
			{
				counter++;
				if (handCopy[i + 1]->getValue() > val)
					val = handCopy[i + 1]->getValue();

				if (counter == 4)
					break;
			}
			else
			{
				counter = 0;
				val = handCopy[i + 1]->getValue();
			}
		}

		if (counter == 4)
		{
			setScore(ourScore, 4);
			ourScore.Strength[0] = val;
		}
	}

	std::vector<int>::iterator flushPos = std::find(countBySuit.begin(), countBySuit.end(), 5);
	std::vector<int>::iterator flushPos2 = std::find(countBySuit.begin(), countBySuit.end(), 6);
	std::vector<int>::iterator flushPos3 = std::find(countBySuit.begin(), countBySuit.end(), 7); // highly unlikely, but we need to check
	if (flushPos != countBySuit.end() || flushPos2 != countBySuit.end() || flushPos3 != countBySuit.end())
	{
		// hand has to be resorted to get the flush :(
		std::sort(handCopy.begin(), handCopy.end(), [](const CardBase* a, const CardBase* b)
		{
			return a->getSuit() < b->getSuit();
		});

		int counter = 0, val = handCopy[0]->getValue();
		for (int i = 0; i < handCopy.size() - 1; i++)
		{
			if (handCopy[i]->getSuit() == handCopy[i + 1]->getSuit())
			{
				counter++;
				if (handCopy[i + 1]->getValue() > val)
					val = handCopy[i + 1]->getValue();

				if (counter == 4)
					break;
			}
			else
			{
				counter = 0;
				val = handCopy[i + 1]->getValue();
			}
		}
		int handTemp = ourScore.Hand;

		// we know we have a flush
		setScore(ourScore, 5);
		ourScore.Strength[0] = val;

		// check if we got a straight flush
		if (counter == 4 && handTemp == 4)
			setScore(ourScore, 8);
	}

	std::vector<int>::iterator fourPos = std::find(countByRank.begin(), countByRank.end(), 4);
	if (fourPos != countByRank.end())
	{
		int index = std::distance(countByRank.begin(), fourPos);
		// dont need to check anything else this beats everything before us
		setScore(ourScore, 7);
		ourScore.Strength[0] = index;
	}

	// check if we have not made any hand
	if (ourScore.Hand == 0 && privateCards.size() > 0)
	{
		// what is the highest card?
		ourScore.Strength[0] = (privateCards[0]->getValue() > privateCards[1]->getValue()) ? privateCards[0]->getValue() : privateCards[1]->getValue();
	}

	return ourScore;
}

int getHandVariance(std::vector<CardBase>& cards, int start, Score target)
{
	assert(cards.size() == 7);

	if (target.Hand == 0)
		return -1;  // target hand has nothing to aim for, always 100%

	std::vector<CardBase> futureCards(cards.begin() + start, cards.end());
	// get score of what we currently have
	std::vector<CardBase> privateCards(cards.begin(), cards.begin() + 2);

	std::vector<CardBase> publicCards(cards.begin() + 2, cards.begin() + start);
	std::vector<CardBase> currentHand(cards.begin(), cards.begin() + start);

	if (currentHand.size() < 5)
		return -1;

	Score currentScore = getScore(privateCards.data(), publicCards.data(), privateCards.size(), publicCards.size());

	if (currentScore.HandValue == target.HandValue)
		return -1; // we already have our wanted hand, 100%

	int counter = 0, val = currentHand[0].getValue();
	for (int i = 0; i < currentHand.size() - 1; i++)
	{
		if (currentHand[i].getValue() == currentHand[i + 1].getValue() + 1 ||
			currentHand[i].getValue() == currentHand[i + 1].getValue() - 1)
		{
			counter++;
			if (currentHand[i + 1].getValue() > val)
				val = currentHand[i + 1].getValue();

			if (counter == 4)
				break;
		}
		else
		{
			counter = 0;
			val = currentHand[i + 1].getValue();
		}
	}

	// the following calculates the 'outs'
	int outs = 0;
	switch (target.HandValue)
	{
	case 1:
		outs = 6; // 6 different cards to get a pair
		break;
	case 2:
		if (currentScore.HandValue == 1)
			outs = 5;
		else if (futureCards.size() == 2)
			outs = 6;
		else assert(1 == 0); // this should never happen
		break;
	case 3:
		outs = 2;
		break;
	case 4:
		if (counter != 0)
		{
			if (counter == 3)
				outs = 8;
			else outs = 12;
		}	
		else
		{
			if (futureCards.size() == 2)
				outs = 8;
			else outs = 4;
		}
		break;
	case 5:
	{
		std::vector<int> countBySuit;
		countBySuit.resize(4);
		for (int i = 0; i < currentHand.size(); i++)
			countBySuit[currentHand[i].getSuit()] += 1;
		if (std::find(countBySuit.begin(), countBySuit.end(), 4) != countBySuit.end())
			outs = 9;
		else outs = 10;
	}
		break;
	case 6:
		if (currentScore.HandValue == 2)
			outs = 4;
		else if (currentScore.HandValue == 3)
			outs = 6;
		else outs = 5;
		break;
	case 7:
		if (currentScore.HandValue == 3)
			outs = 1;
		else if (currentScore.HandValue == 2)
			outs = 2;
		break;
	case 8:
		if (counter != 0)
		{
			if (counter == 3)
				outs = 2;
			else outs = 3;
		}
		else
		{
			if (futureCards.size() == 2)
				outs = 2;
			else outs = 1;
		}
		break;
	}


	return outs;
}

float outToPercentage(int out, bool turn)
{
	return (out * (turn) ? FLOP_TO_TURN_PERC : TURN_TO_RIVER_PERC) / 100.f; // convert to decimal
}

void setScore(Score& score, int handvalue)
{
	switch (handvalue)
	{
	case 0:
		score.Hand = 0.f;
		score.HandValue = 0;
		break;
	case 1:
		score.Hand = PAIR_VALUE;
		score.HandValue = 1;
		break;
	case 2:
		score.Hand = TWOPAIR_VALUE;
		score.HandValue = 2;
		break;
	case 3:
		score.Hand = THREEKIND_VALUE;
		score.HandValue = 3;
		break;
	case 4:
		score.Hand = STRAIGHT_VALUE;
		score.HandValue = 4;
		break;
	case 5:
		score.Hand = FLUSH_VALUE;
		score.HandValue = 5;
		break;
	case 6:
		score.Hand = FULL_VALUE;
		score.HandValue = 6;
		break;
	case 7:
		score.Hand = FOUR_VALUE;
		score.HandValue = 7;
		break;
	case 8:
		score.Hand = RUN_VALUE;
		score.HandValue = 8;
		break;
	default:
		score.Hand = 0;
		score.HandValue = 0;
		break;
	}
}