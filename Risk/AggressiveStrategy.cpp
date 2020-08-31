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

#include "AggressiveStrategy.h"
#include "Country.h"

AggressiveStrategy::AggressiveStrategy(shared_ptr<Player> player) : Strategy(player), m_maxAttackingCountryID(0)
{
}

void AggressiveStrategy::reinforce(int continentBonus)
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

	// find the strongest country that the player owns (or first strongest if there is a tie)
	vector<shared_ptr<Country>> countries = m_player->getCountries();
	int strongestCountryStrength = 0;
	int strongestID = 0;
	if (countries.size() > 0) {
		for (auto &iter : countries) {

			if (iter->getArmyValue() > strongestCountryStrength) {
				strongestCountryStrength = iter->getArmyValue();
				strongestID = iter->getCountryId();
			}
		}

		//cout << "Choose a country (by ID) to add reinforcements to: ";
		auto country = m_player->findCountry(countries, strongestID);
		if (country) {
			country->addToArmyValue(reinforcementsGained);
			cout << m_player->name() << " reinforced " << country->getName() << " with " << reinforcementsGained << " armies" << endl;

			m_player->setReinforcements(0);
		}
		else {
			cout << "Country to reinforce not found!" << endl;
		}
	}
}
void AggressiveStrategy::attack()
{
	vector<shared_ptr<Country>> countries = m_player->getCountries();
	if (countries.size() > 0) {
		bool attacking = true;

		// as long as the Player chooses to continue attacking
		while (attacking) {

			// a Player can attack if they have a Country with armyValue > 2
			bool canAttack = false;

			// find the strongest country that the player owns (or first strongest if there is a tie)
			int maxArmyValue = 0;
			int maxAttackingCountryID = -1;
			for (auto &iter : countries) {
				if (iter->getArmyValue() >= 2) {
					// found at least 1 country that can attack
					if (iter->getArmyValue() > maxArmyValue) {
						maxArmyValue = iter->getArmyValue();
						maxAttackingCountryID = iter->getCountryId();
					}
					canAttack = true;
				}
			}

			auto attackingCountry = m_player->findCountry(countries, maxAttackingCountryID);
			
			// If user entered a valid ID for an attacking country
			if (attackingCountry) {

				while (canAttack) {
					// get adjacent countries vector (some of the adjacent countries might be owned by this player)
					// so search vector for countries owned by another player and display them as possible attack targets
					// push the IDs of those countries into a vector of possible targets
					auto adjacentCountries = attackingCountry->getAdjCountries();
					vector<shared_ptr<Country>> possibleTargets;

					for (auto &iter : adjacentCountries) {
						if (iter->getOwnersId() != m_player->id()) {
							// IDs don't match, possible target found
							possibleTargets.push_back(iter);
						}
					}

					if (!possibleTargets.empty()) {

						auto defendingCountry = possibleTargets.back();
						if (defendingCountry) {

							cout << m_player->name() << " attacks " << defendingCountry->getName() << " from " << attackingCountry->getName() << endl;

							// determing how many dice the attacker can roll
							int atkArmyValue = attackingCountry->getArmyValue();
							int atkDice = (atkArmyValue > 3 ? 3 : (atkArmyValue - 1));

							// determine how many dice the defender can roll
							shared_ptr<Player> defPlayer = defendingCountry->getOccupyingPlayer();
							int defArmyValue = defendingCountry->getArmyValue();
							int defDice = defPlayer->chooseDefensiveDice(defendingCountry);

							// roll dice
							// compare results and determine victor (remove armies accordingly), transfer player ownership if required
							vector<int> atkResults = m_player->roll(atkDice);
							vector<int> defResults = defPlayer->roll(defDice);
							bool conquered = false;

							// attacker's roll results
							cout << m_player->name() << " rolled ";
							for (auto &i : atkResults)
								cout << i << " ";
							cout << endl;

							// defender's roll results
							cout << defPlayer->name() << " rolled ";
							for (auto &i : defResults)
								cout << i << " ";
							cout << endl << endl;

							// get minimum size to iterate to for comparison (size of their result vectors may be different), to avoid out of bounds
							int min = (atkResults.size() < defResults.size() ? atkResults.size() : defResults.size());

							// compare results
							for (int i = 0; i < min; i++) {
								// attacker wins if roll is higher
								if (atkResults[i] > defResults[i]) {
									defArmyValue--;
									if (defArmyValue <= 0) {
										// defender has no armies left
										conquered = true;

										// pop the back element from possible targets (back element was originally chosen as the target)
										// we want to force the player to choose another country to attack from if there are no adjacent countries to attack.
										possibleTargets.pop_back();

										cout << defendingCountry->getName() << " has been conquered!" << endl;
										cout << "Switching ownership from " << defendingCountry->getOccupyingPlayer()->name() << " to " << m_player->name() << endl << endl;

										// Aggressive player will only move 1 army in order to maintain the maximum # of armies on their strongest country
										int armiesToMove = 1;

										//cout << "Maximizing aggregation of armies, player moves 1 army from " << attackingCountry->getName() << " to " << defendingCountry->getName() << endl;

										defendingCountry->setArmyValue(armiesToMove);
										attackingCountry->setArmyValue(atkArmyValue - armiesToMove);

										// switch ownership
										// we use the erase / remove idiom to remove it from the defending player's vector of countries (in Player::removeCountry(int))
										// then set that countries owner to this player, and add it to this player's vector of countries
										int defendingCountryId = defendingCountry->getCountryId();
										defPlayer->removeCountry(defendingCountryId);
										defendingCountry->setOccupyingPlayer(m_player);
										m_player->addCountry(defendingCountry);
										break;
									}
								}
								// defender wins on tie or better
								else {
									atkArmyValue--;
								}
							} // for

							// If the country was conquered we set the armyValues in the for loop above, otherwise do it here
							if (!conquered) {
								attackingCountry->setArmyValue(atkArmyValue);
								defendingCountry->setArmyValue(defArmyValue);
							}

							// let player attack again from same territory if they still have enough armies
							if (attackingCountry->getArmyValue() > 1) {
								if (possibleTargets.empty()) {
									canAttack = false;
									attacking = false;
									cout << "There are no targets adjacent to that country!" << endl;
								}
							}
							else {
								canAttack = false;
								attacking = false;
								cout << "Attacker does not have enough armies to continue attacking with this country" << endl;
							}
						} // if(defendingCountry)
						else {
							cout << "Could not find the defending country!" << endl;
						}
					} // if(!possibleTargets.empty()
					else {
						attacking = false;
						cout << "There are no targets adjacent to that country!" << endl;
						return;
					}
				} // while(canAttack)
			} // if(attackingCountry)
			else {
				cout << "Could not find the attacking country!" << endl;
				attacking = false;
			}
		} // while(attacking)
	} // if(countries.size() > 0)
}
void AggressiveStrategy::fortify()
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

		// find the strongest country that the player owns (or first strongest if there is a tie)
		int maxArmyValue = 0;
		for (auto &iter : countries) {
			if (iter->getArmyValue() > maxArmyValue) {
				maxArmyValue = iter->getArmyValue();
				m_maxAttackingCountryID = iter->getCountryId();
			}
		}

		auto targetCountry = m_player->findCountry(countries, m_maxAttackingCountryID);
		if (targetCountry) {
			if (canFortify) {
				auto adjacentCountries = targetCountry->getAdjCountries();

				for (auto &iter : adjacentCountries) {
					if (iter->getOwnersId() == m_player->id()) {
						if (iter->getArmyValue() > 1) {
							int armiesToMove = iter->getArmyValue() - 1;
							m_player->moveArmies(iter, targetCountry, armiesToMove);
							cout << endl;
							cout << "Fortification Result" << endl;
							cout << "================" << endl;
							cout << iter->getName() << " has moved " << armiesToMove << " armies successfully." << endl;
							cout << iter->getName() << " now has: " << iter->getArmyValue() << endl;
							cout << targetCountry->getName() << " now has: " << targetCountry->getArmyValue() << endl;
							cout << "The fortification phase is over" << endl << endl;
							break;
						}
					}
				}
			}
		}
		else {
			cout << "The strongest country could not be found" << endl;
		}
	}
}

int AggressiveStrategy::chooseDefensiveDice(shared_ptr<Country> &country)
{
	// Get the army value of the defending country
	// An aggressive player will always defend with the maximum possible dice
	int defArmyValue = country->getArmyValue();
	return defArmyValue > 2 ? 2 : defArmyValue;
}

AggressiveStrategy::~AggressiveStrategy()
{
}

