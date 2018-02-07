#include "Utility.h"
#include "Card.h"

// implemented by the chen formula - made by Bill Chen
// this formula is for first play only, it is to give an idea of how good
// a starting hand is
signed int chen(CardBase f, CardBase s)
{
	float score = 0.f;

	int fValue = f.getValue();
	int sValue = s.getValue();

	int high = (fValue > sValue) ? fValue : sValue;

	// get card value;
	switch (high)
	{
	case 13:
		score = 10.f;
		break;
	case 12:
		score = 8.f;
		break;
	case 11:
		score = 7.f;
		break;
	case 10:
		score = 6.f;
		break;
	default:
		score = (high + 1) / 2.f;
		break;
	}

	// discover pairs
	if (fValue == sValue)
		score *= 2.f;
	// discover gap
	else if (((fValue - 1) == sValue) || ((fValue + 1) == sValue))
		score += 1;
	else if (((fValue - 2) == sValue) || ((fValue + 2) == sValue))
		score -= 1;
	else if (((fValue - 3) == sValue) || ((fValue + 3) == sValue))
		score -= 2;
	else if (((fValue - 4) == sValue) || ((fValue + 4) == sValue))
		score -= 4;
	else
		score -= 5;

	// same suit
	if (f.getSuit() == s.getSuit())
		score += 2;	

	return score;
}