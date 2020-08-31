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

#include "PassiveStrategy.h"
#include "Country.h"

PassiveStrategy::PassiveStrategy(shared_ptr<Player> player) : Strategy(player)
{
}

void PassiveStrategy::reinforce(int continentBonus)
{
	// award reinforcements equal to # of countries owned / 3 (minimum 3)
	unsigned int reinforcementsGained = m_player->getCountries().size() / 3;

	// check for owning entire continent to award additional reinforcements
	reinforcementsGained += continentBonus;

	// allow player to exchange cards for armies (5 first time, +5 each time after)
	// force exchange if hand size > 5
	unsigned int exchangeResult = 0;
	do {
		exchangeResult = m_player->exchange();
		reinforcementsGained += exchangeResult;
	} while (m_player->getHandSize() > 5);

	// award minimum 3 after this
	m_player->setReinforcements((reinforcementsGained > 3) ? reinforcementsGained : 3);
	cout << "Total reinforcements for this turn are: " << reinforcementsGained << endl << endl;

	// find the weakest country that the player owns (or first weakest if there is a tie)
	vector<shared_ptr<Country>> countries = m_player->getCountries();
	if (countries.size() > 0) {
		int weakestCountryStrength = countries.front()->getArmyValue();
		int weakestID = countries.front()->getCountryId();
		while (reinforcementsGained > 0) {
			for (auto &iter : countries) {
				if (iter->getArmyValue() < weakestCountryStrength) {
					weakestID = iter->getCountryId();
				}
			}

			auto country = m_player->findCountry(countries, weakestID);
			if (country) {
				country->addToArmyValue(1);
				cout << m_player->name() << " reinforced " << country->getName() << " with 1 army" << endl;
				reinforcementsGained--;
				for (auto &iter : countries) {
					if (iter->getArmyValue() < country->getArmyValue() && reinforcementsGained > 0) {
						iter->addToArmyValue(1);
						cout << m_player->name() << " reinforced " << iter->getName() << " with 1 army" << endl;
						reinforcementsGained--;
					}
				}
			}
			else {
				cout << "Country not found!" << endl;
			}
		}
		m_player->setReinforcements(0);
	}
}

void PassiveStrategy::attack()
{
	cout << "A passive player never attacks" << endl;
}

void PassiveStrategy::fortify()
{
	vector<shared_ptr<Country>> countries = m_player->getCountries();
	if (countries.size() > 0) {

		// a Player can fortify if they have a Country with armyValue > 2
		bool canFortify = false;
		for (auto &iter : countries) {
			if (iter->getArmyValue() >= 2) {
				// found at least 1 country that can fortify
				canFortify = true;
			}
		}

		int weakestCountryStrength = countries.front()->getArmyValue();
		int weakestID = countries.front()->getCountryId();
		for (auto &iter : countries) {
			if (iter->getArmyValue() < weakestCountryStrength) {
				weakestID = iter->getCountryId();
			}
		}

		auto country = m_player->findCountry(countries, weakestID);
		if (country) {
			auto adjacentCountries = country->getAdjCountries();

			int delta = 2;
			for (auto &iter : adjacentCountries) {
				if (iter->getOwnersId() == m_player->id()) {
					if (iter->getArmyValue() - country->getArmyValue() > delta) {
						int armiesToMove = delta / 2;
						m_player->moveArmies(iter, country, armiesToMove);
						cout << endl;
						cout << "Fortification Result" << endl;
						cout << "================" << endl;
						cout << iter->getName() << " has moved " << armiesToMove << " armies successfully." << endl;
						cout << iter->getName() << " now has: " << iter->getArmyValue() << endl;
						cout << country->getName() << " now has: " << country->getArmyValue() << endl;
						cout << "The fortification phase is over" << endl << endl;
						break;
					}
				}
			}
		}
		else {
			cout << "The weakest country could not be found" << endl;
		}
	}
}

int PassiveStrategy::chooseDefensiveDice(shared_ptr<Country>& country)
{
	// A passive player always defends with the least amount of dice
	return 1;
}

PassiveStrategy::~PassiveStrategy()
{
}
