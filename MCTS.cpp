#include "MCTS.h"
#include "Node.h"
#include "GameState.h"
#include "Player.h"
#include <time.h>

#include "Card.h"

#define CHANCE_MOD 0.1f

int randomRange(int min, int max)
{
	return min + (rand() % (max - min + 1));
}

GameState* MCTS_BestMove(GameState* game)
{
	Node* root = new Node(new GameState(game));
	// force gamestate to calculate remaining cards
	game->availableCards();

	for (int i = 0; i < 1000; i++)
	{
		Node* current = MCTS_Selection(root, game);
		float value = MCTS_Simulate(current, game);
		MCTS_Backpropagate(current, value);
	}

	if (root->getChildCount() == 0)
	{
		return root->getState();
	}

	Node* mct_best = MCTS_UCTPlus(root, 0, 2.8f); // UCTPlus to give high probability a high impact
	GameState* returnState = new GameState(mct_best->getState());

	// we need to know if the gamestate is valuable enough for play
	returnState->setValue(mct_best->getValue());

	delete root;

	return returnState;
}

Node* MCTS_Selection(Node* current, GameState* game)
{
	while (!current->isTerminal())
	{
		GameState* gameToUse = current->getState();
		int remainingNodes = gameToUse->getRemainingCardCount();

		if (remainingNodes > current->children().size())
			return MCTS_Expand(current, gameToUse);
		else
			current = MCTS_UCT(current, 1.23f);
	}

	return current;
}

Node* MCTS_UCT(Node* current, float C)
{
	Node* childret = NULL;
	float best = -1.f;

	for (int i = 0; i < current->children().size(); i++)
	{
		Node* childNode = current->children()[i];
		float ucb = (childNode->getValue() / childNode->getVisits()) + C * sqrt((2 * log(current->getVisits())) / childNode->getVisits());

		if (ucb > best)
		{
			childret = childNode;
			best = ucb;
		}
	}

	return childret;
}

Node* MCTS_UCTPlus(Node* current, float C1, float C2)
{
	Node* childret = NULL;
	float best = -1.f;

	for (int i = 0; i < current->children().size(); i++)
	{
		Node* childNode = current->children()[i];
		float ucb = (childNode->getValue() / childNode->getVisits()) + C1 * sqrt((2 * log(current->getVisits())) / childNode->getVisits()) + C2 * childNode->getProbability();

		if (ucb > best)
		{
			childret = childNode;
			best = ucb;
		}
	}

	return childret;
}

Node* MCTS_Expand(Node* current, GameState* game)
{
	int remainingNodes = game->getRemainingCardCount();

	// for each valid move
	for (int i = 0; i < remainingNodes; i++)
	{
		GameState* next = new GameState(game); // create clone
		next->addCardToGame(game->getNextCard(true));

		Node* n = new Node(next);
		n->setProbability(1.f / next->getRemainingCardCount());

		current->addChild(n);
		
		return n;
	}

	return NULL;
}

float MCTS_Simulate(Node* current, GameState* rootState)
{
	srand(time(NULL));

	GameState* next = new GameState(current->getState()); // create clone

	while (!next->isTerminalState())
	{
		std::vector<CardBase> remainingCards = next->availableCards();
		CardBase card = remainingCards[randomRange(0, remainingCards.size() - 1)];
		next->addCardToGame(card);
	}

	// get score for this gamestate
	// we will use the hand strength for this
	std::vector<CardBase> allCards = next->getCards();
	// first 2 cards are private - in case of texas holdem
	std::vector<CardBase> privateCards;
	privateCards.push_back(allCards[0]);
	privateCards.push_back(allCards[1]);
	// rest are community cards
	std::vector<CardBase> publicCards(allCards.begin() + 2, allCards.end());
	Score handScore = getScore(privateCards.data(), publicCards.data(), 2, publicCards.size());
	Score publicScore = getScore(NULL, publicCards.data(), 0, publicCards.size());

	// calculate variance of hand
	int variance = getHandVariance(allCards, rootState->getCards().size(), handScore);

	float perc = 1.f;
	if (variance > 0)
		perc = outToPercentage(variance, (rootState->getCards().size() == 5) ? true : false);

	// add extra to modify value
	if (perc + CHANCE_MOD > 1.f)
		perc = 1.f;
	else perc += CHANCE_MOD;

	// if our private cards dont add to our result, mark this a bad hand
	if (handScore == publicScore)
		perc = 0.f;

	//delete next;

	return handScore.Hand * perc;
}

void MCTS_Backpropagate(Node* current, float value)
{
	while (current != NULL)
	{
		current->visit();
		current->addValue(value);
		current = current->getParent();
	}
}