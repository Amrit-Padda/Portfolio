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

#ifndef RISK_HAND_H
#define RISK_HAND_H

#include "stdafx.h"
#include "Card.h"

using namespace std;

// a class to represent a hand of Risk cards
// each player has their own hand of cards
// cards in a hand can be exchanged for additional armies
// a player can exchange 3 of the same type, or 1 of each of artillery, cavalry and infantry
// recently exchanged cards are passed up through the player object to the game object where they can be put back into the deck
class Hand {
public:
	void	draw(shared_ptr<Card> &);
	vector<shared_ptr<Card>> mostRecentlyExchanged();
    int		exchange();
	vector<shared_ptr<Card>>	getCards() const;
	void	showHand() const;
	int		size() const;
	void	clear();
private:
	vector<shared_ptr<Card>>	m_cards;
	vector<shared_ptr<Card>>	m_exchangedCards;
	int							m_totalExchanges = 0;

    bool allSame(vector<shared_ptr<Card>>);
    bool allDifferent(vector<shared_ptr<Card>>);
	bool containsWildcard(vector<shared_ptr<Card>>);
};

#endif //RISK_HAND_H
