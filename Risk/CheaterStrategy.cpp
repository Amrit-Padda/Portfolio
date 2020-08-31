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

#include "CheaterStrategy.h"
#include "Country.h"

CheaterStrategy::CheaterStrategy(shared_ptr<Player> player) : Strategy(player)
{
}

void CheaterStrategy::reinforce(int continentBonus)
{
	m_player->exchange();

	//Cheater doubles the army value of all its countries
	vector<shared_ptr<Country>> countries = m_player->getCountries();
	if (countries.size() > 0) {
		for (auto country : countries) {
			int armyV = country->getArmyValue();
			country->setArmyValue(2 * armyV);
			cout << m_player->name() << " doubled the army value of " << country->getName() << " from " << armyV << " to " << (2 * armyV) << endl;
		}
	}

	m_player->setReinforcements(0);
}

void CheaterStrategy::attack()
{
	//Conquer every country adjacent to his countries
	auto countries = m_player->getCountries();
	if (countries.size() > 0) {
		for (auto &country : countries) {
			auto adjCountries = country->getAdjCountries();

			for (auto &adjCountry : adjCountries) {
				//If the adj country isn't owned by the player already, the cheater now owns it
				if (adjCountry->getOwnersId() != country->getOwnersId()) {
					auto adjPlayerOwner = adjCountry->getOccupyingPlayer();

					cout << adjCountry->getName() << " has been conquered!" << endl;
					cout << "Switching ownership from " << adjPlayerOwner->name() << " to " << m_player->name() << endl << endl;

					adjPlayerOwner->removeCountry(adjCountry->getCountryId());
					adjCountry->setOccupyingPlayer(m_player);
					m_player->addCountry(adjCountry);
				}
			}
		}
	}
}

void CheaterStrategy::fortify()
{
	auto countries = m_player->getCountries();
	if (countries.size() > 0) {

		//If the adj countries of a country is owned by another player, double the cheaters army value for that country.
		for (auto &country : countries) {
			auto adjCountries = country->getAdjCountries();

			for (auto adjCountry : adjCountries) {

				//If the owner is different, double the army value and go to the next country
				if (adjCountry->getOwnersId() != country->getOwnersId()) {
					cout << m_player->name() << " fortified " << country->getName() << " by doubling the army value from " << country->getArmyValue() << " to " << (2 * country->getArmyValue()) << endl;
					country->setArmyValue(country->getArmyValue() * 2);
					break;
				}
			}
		}
	}
}

int CheaterStrategy::chooseDefensiveDice(shared_ptr<Country>& country)
{
	// Cheater always defends with max dice no matter what
	return 3;
}

CheaterStrategy::~CheaterStrategy()
{
}
