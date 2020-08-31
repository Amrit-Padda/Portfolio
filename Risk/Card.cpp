/*
	COMP 345 D Fall 2018
	Assignment 4
	Written by Team #9:
		Amritpal Padda	40042768
		Francois Roy	40065497
		Jason Brennan	27793928
		Olivier Hébert	40051654

	Submitted on November 29, 2018
*/

#include "Card.h"

Card::Card() {

}

Card::Card(Card *c)
{
	type = c->type;
}

Card::Card(CardType t) {
	type = t;
}

void Card::setType(CardType t)
{
	type = t;
}

CardType Card::getType() const
{
	return type;
}

std::string Card::printType() const
{
	switch (type) {
	case CardType::Artillery: return "-Artillery-";
	case CardType::Cavalry: return "-Cavalry-";
	case CardType::Infantry: return "-Infantry-";
	default: return NULL;
	}
}
