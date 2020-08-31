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

#ifndef RISK_DECK_H
#define RISK_DECK_H

#include "stdafx.h"
#include "Card.h"

using namespace std;

// A class to represent a deck of cards in the game Risk
// a deck has a collection of Cards
// a deck has methods to draw cards from and put cards back to the deck
class Deck {
public:
	Deck();
	Deck(int);

	void				init(int);
	void				setNumOfCards(int);
	int					getNumOfCards() const;
	shared_ptr<Card>	draw();
	void 				put_back(vector<shared_ptr <Card>> cards);
	
	int		size() const;
	bool	empty() const;
	void	clear();

private:
	int m_numOfCards;
	vector<shared_ptr<Card>> m_cards;
};

#endif //RISK_DECK_H