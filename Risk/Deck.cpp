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

#include "Deck.h"

Deck::Deck() {

}

Deck::Deck(int n) {
	m_numOfCards = n;
}

void Deck::setNumOfCards(int n) {
	m_numOfCards = n;
}

int Deck::getNumOfCards() const {
	return m_numOfCards;
}

// a card is drawn at random from the deck to simulate drawing the top card of a shuffled deck
shared_ptr<Card> Deck::draw() {
	unsigned int position = 0;
	position = rand() % Deck::m_cards.size();
	auto pick = make_shared<Card>(m_cards.at(position)->getType());
	m_cards.erase(m_cards.begin() + position);

	return pick;
}

// cards exchanged for armies by a player are put back in the deck
void Deck::put_back(vector<shared_ptr <Card>> cards) {
	int loops = cards.size();
	for(int i = 0; i < loops; ++i){
		m_cards.push_back(cards.at(i));

	}
}

// create a deck with size equal to number of countries
// the deck is not shuffled, instead, cards are picked fromt he deck at random (see Deck::draw())
void Deck::init(int nbCountries) {
	
	for(int i = 0; i < nbCountries; i++) {
		auto c = make_shared<Card>();
		switch(i % 3) {
		case 0:
			c->setType(CardType::Artillery);
			break;
		case 1:
			c->setType(CardType::Infantry);
			break;
		case 2:
			c->setType(CardType::Cavalry);
			break;
		}
		m_cards.push_back(c);
	}
}

int Deck::size() const
{
	return m_cards.size();
}

bool Deck::empty() const
{
	return m_cards.empty();
}

void Deck::clear()
{
	m_numOfCards = 0;
	m_cards.clear();
}
