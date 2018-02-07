#include "Deck.h"
#include "Card.h"

// cards in the deck
#define DECK_LIMIT 52
// how many cards are shuffled
#define SHUFFLE_COUNT 1000

Deck::Deck(cocos2d::CCLayer* scene)
	: m_deckCounter(DECK_LIMIT-1)
{
	// alloc deck
	m_deck = new Card*[52];

	// assemble deck
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 13; j++)
		{
			std::string name = "./cards/";
			Suit suit = (Suit)i;

			char c;

			// get asset art name
			switch (j + 1)
			{
			case 1:
				name += "ace";
				break;
			case 11:
				name += "jack";
				break;
			case 12:
				name += "queen";
				break;
			case 13:
				name += "king";
				break;
			default:
				name += std::to_string(j + 1);
				break;
			}

			name += "_of_";

			switch (suit)
			{
			case Clubs:
				name += "clubs";
				break;
			case Spades:
				name += "spades";
				break;
			case Diamonds:
				name += "diamonds";
				break;
			case Hearts:
				name += "hearts";
				break;
			}

			name += ".png";

			// check for ace, we want to give it a high value (ace high)
			Card* card = new Card(name.c_str(), suit, (j == 0)?13:j);
			card->getAvatar()->setVisible(false);

			scene->addChild(card);

			m_deck[j + (i * 13)] = card;
		}
}

void Deck::shuffle()
{
	srand(time(NULL));

	for (int i = 0; i < SHUFFLE_COUNT; i++)
	{
		int card = rand() % DECK_LIMIT;
		int card2 = rand() % DECK_LIMIT;
		
		if (card == card2)
			continue;
		Card* temp = m_deck[card2];
		m_deck[card2] = m_deck[card];
		m_deck[card] = temp;
	}

	m_deckCounter = DECK_LIMIT - 1;
}

Card* Deck::drawCard()
{
	if (m_deckCounter == 0)
		return NULL;

	return m_deck[m_deckCounter--];
}

Deck::~Deck()
{
	delete[] m_deck;
}