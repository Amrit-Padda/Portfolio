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

#include "Hand.h"

void Hand::draw(shared_ptr<Card> &c)
{
	m_cards.push_back(c);
}

vector<shared_ptr<Card>> Hand::mostRecentlyExchanged() {
	return m_exchangedCards;
}

// Find all possible exchanges a player could make, based on the cards in hand
// award armies at 5 times the amount of exchanges so far
int Hand::exchange() {

	vector<vector<unsigned int>> possibleExchanges;

	if (m_cards.size() >= 3) {
		if (m_cards.size() == 3) {
			if (allSame(m_cards) || allDifferent(m_cards)) {

				m_exchangedCards.clear();
				auto usedCard = make_shared<Card>(m_cards.at(0).get());
				m_exchangedCards.push_back(usedCard);
				usedCard = make_shared<Card>(m_cards.at(1).get());
				m_exchangedCards.push_back(usedCard);
				usedCard = make_shared<Card>(m_cards.at(2).get());
				m_exchangedCards.push_back(usedCard);

				m_cards.erase(m_cards.begin(), m_cards.begin() + 3);
				++m_totalExchanges;
				return 5 * m_totalExchanges;
			}
		}
		else if (m_cards.size() <= 6) {

			unsigned long size = m_cards.size();

			for (unsigned int i = 0; i < size - 2; ++i) {
				for (unsigned int j = i + 1; j < size - 1; ++j) {
					for (unsigned int k = j + 1; k < size; ++k) {
						vector<shared_ptr<Card>> subset = { m_cards.at(i), m_cards.at(j), m_cards.at(k) };
						if (allSame(subset) || allDifferent(subset) || containsWildcard(subset)) {
							vector<unsigned int> v{ i, j, k };
							possibleExchanges.push_back(v);
						}
					}
				}
			}

			if (possibleExchanges.empty()) {
				//std::cout << "No exchange possible" << std::endl;
				return 0;
			}

			//Removed user choice, automatically goes with first combo
			/*
			std::cout << "Choose a combination to exchange " << std::endl;
			for (unsigned int i = 0; i < possibleExchanges.size(); ++i) {
				std::string output = ". ";
				for (int j = 0; j < 3; ++j) {
					switch (m_cards.at(possibleExchanges.at(i)[j])->getType()) {
					case CardType::Artillery: output += "-Artillery-"; break;
					case CardType::Cavalry: output += "-Cavalry-"; break;
					case CardType::Infantry: output += "-Infantry-"; break;
					default: output += "";
					}
				}

				std::cout << i + 1 << output << std::endl;
			}
			int choice = -1;

			do {
				cout << "Enter number : ";
				string temp;
				cin >> temp;
				stringstream(temp) >> choice;

			} while (choice < 0 || choice > (int)possibleExchanges.size());
			*/

			int choice = 1;
			// store the exchanged cards in a vector
			// these cards will be passed up from hand -> player -> game, and then down from game -> deck
			m_exchangedCards.clear();
			auto usedCard = make_shared<Card>(m_cards.at(possibleExchanges.at(choice - 1)[0]).get());
			m_exchangedCards.push_back(usedCard);
			usedCard = make_shared<Card>(m_cards.at(possibleExchanges.at(choice - 1)[1]).get());
			m_exchangedCards.push_back(usedCard);
			usedCard = make_shared<Card>(m_cards.at(possibleExchanges.at(choice - 1)[2]).get());
			m_exchangedCards.push_back(usedCard);

			// We'll need to calculate an offset to remove cards after the first since the size of the vector shrinks with each removal
			int offset = 0;
			m_cards.erase(m_cards.begin() + possibleExchanges.at(choice - 1)[0]);
			offset = (possibleExchanges.at(choice - 1)[1]) - 1; // removed 1 card so far
			m_cards.erase(m_cards.begin() + offset);
			offset = (possibleExchanges.at(choice - 1)[2]) - 2; // removed 2 cards so far
			m_cards.erase(m_cards.begin() + offset);

			++m_totalExchanges;
			return 5 * m_totalExchanges;
		}
	}
	return 0;
}

vector<shared_ptr<Card>> Hand::getCards() const
{
	return m_cards;
}

void Hand::showHand() const
{
	for (auto &card : m_cards)
		cout << card->printType() << endl;
}

int Hand::size() const
{
	return m_cards.size();
}

void Hand::clear()
{
	m_cards.clear();
	m_exchangedCards.clear();
	m_totalExchanges = 0;
}

bool Hand::allSame(vector<shared_ptr<Card>> set) {
	return set.at(0)->getType() == set.at(1)->getType() && set.at(1)->getType() == set.at(2)->getType();
}

bool Hand::containsWildcard(vector<shared_ptr<Card>> set) {
	return set.at(0)->getType() == CardType::Wildcard || set.at(1)->getType() == CardType::Wildcard || set.at(2)->getType() == CardType::Wildcard;
}

bool Hand::allDifferent(vector<shared_ptr<Card>> set) {
	return set.at(0)->getType() != set.at(1)->getType() && set.at(1)->getType() != set.at(2)->getType() && set.at(0)->getType() != set.at(2)->getType();
}