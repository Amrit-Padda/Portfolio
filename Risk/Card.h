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

#ifndef RISK_CARD_H
#define RISK_CARD_H

#include "stdafx.h"

// Types of cards
enum CardType { Infantry = 1, Artillery = 3, Cavalry = 5, Wildcard = 7 };

// A class to represent a card in the game Risk
// a card has 1 of 4 types denoted by the enum CardType defined above
class Card {
public:
	Card();
	Card(Card *);
	explicit Card(CardType);
	void setType(CardType);
	CardType getType() const;
	std::string printType() const;
private:
	CardType type;
};

#endif //RISK_CARD_H