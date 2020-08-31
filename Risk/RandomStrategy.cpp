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

#include "RandomStrategy.h"
#include "Country.h"

RandomStrategy::RandomStrategy(shared_ptr<Player> player) : Strategy(player)
{
}

void RandomStrategy::reinforce(int continentBonus)
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
		while (m_player->getReinforcements() > 0) {
			int reinforcedCountryIndex = rand() % countries.size();
			int reinforcementsToAdd = (rand() % m_player->getReinforcements()) + 1;
			countries.at(reinforcedCountryIndex)->addToArmyValue(reinforcementsToAdd);
			m_player->addReinforcements(-reinforcementsToAdd);

			cout << m_player->name() << " reinforced " << countries.at(reinforcedCountryIndex)->getName() << " with " << reinforcementsToAdd << " armies" << endl;
		}
	}
}

void RandomStrategy::attack()
{
	vector<shared_ptr<Country>> countries = m_player->getCountries();
	if (countries.size() > 0) {
		int willAttack = rand() % 2;
		bool attacking = (willAttack == 1 ? true : false);

		// as long as the Player chooses to continue attacking
		while (attacking) {

			// a Player can attack if they have a Country with armyValue > 2
			bool canAttack = false;
			vector<shared_ptr<Country>> possibleAttackers;
			for (auto &iter : countries) {
				if (iter->getArmyValue() >= 2) {
					// found at least 1 country that can attack
					possibleAttackers.push_back(iter);
					canAttack = true;
				}
			}

			if (possibleAttackers.size() > 0) {
				// find a random country that the player owns 
				int attackingCountryIndex = rand() % possibleAttackers.size();
				auto attackingCountry = possibleAttackers.at(attackingCountryIndex);

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

						if (possibleTargets.size() > 0) {

							int defendingCountryIndex = rand() % possibleTargets.size();
							auto defendingCountry = possibleTargets.at(defendingCountryIndex);
							if (defendingCountry) {

								cout << m_player->name() << " attacks " << defendingCountry->getName() << " from " << attackingCountry->getName() << endl;

								// randomly determine how many dice the attacker can roll
								int atkArmyValue = attackingCountry->getArmyValue();
								int possibleDice = (atkArmyValue > 3 ? 3 : (atkArmyValue - 1));
								int atkDice = (rand() % possibleDice) + 1;

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

											// remove the conquered country from list of possible targets
											possibleTargets.erase(possibleTargets.begin() + defendingCountryIndex);

											cout << defendingCountry->getName() << " has been conquered!" << endl;
											cout << "Switching ownership from " << defendingCountry->getOccupyingPlayer()->name() << " to " << m_player->name() << endl << endl;

											// Random player will only move random number of armies
											int armiesToMove = (rand() % (attackingCountry->getArmyValue() - 1)) + 1;

											defendingCountry->setArmyValue(armiesToMove);
											attackingCountry->addToArmyValue(-armiesToMove);

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
								} // for dice comparisons

								// If the country was conquered we set the armyValues in the for loop above, otherwise do it here
								if (!conquered) {
									attackingCountry->setArmyValue(atkArmyValue);
									defendingCountry->setArmyValue(defArmyValue);
								}

								cout << "Country Status" << endl;
								cout << attackingCountry->getName() << " has " << attackingCountry->getArmyValue() << " armies left" << endl;
								cout << defendingCountry->getName() << " has " << defendingCountry->getArmyValue() << " armies left" << endl << endl;

								// let player attack again from same territory if they still have enough armies
								if (attackingCountry->getArmyValue() > 1) {
									int ans = rand() % 2;
									canAttack = (ans == 1 ? true : false);
									
									if (possibleTargets.empty()) {
										canAttack = false;
										cout << "There are no targets adjacent to that country!" << endl;
									}
								}
								else {
									cout << "Attacker does not have enough armies to continue attacking with this country" << endl;
									canAttack = false;
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
				}
				// let player attack with another country until they decide to stop
				int continueAttacking = rand() % 2;
				attacking = (continueAttacking == 1 ? true : false);
			} //if (possibleAttackers.size() > 0)
			else {
				attacking = false;
			}
		} // while(attacking)
	} // if(countries.size() > 0)
}

void RandomStrategy::fortify()
{
	vector<shared_ptr<Country>> countries = m_player->getCountries();
	if (countries.size() > 0) {

		// a Player can fortify if they have a Country with armyValue > 2
		bool canFortify = false;
		vector<shared_ptr<Country>> possibleSources;
		for (auto &iter : countries) {
			if (iter->getArmyValue() >= 2) {
				// found at least 1 country that can fortify
				possibleSources.push_back(iter);
				canFortify = true;
			}
		}

		if (possibleSources.size() > 0) {
			int fortifySourceIndex = rand() % possibleSources.size();
			auto &country = possibleSources.at(fortifySourceIndex);
			if (country) {

				vector<shared_ptr<Country>> possibleTargets;
				auto adjacentCountries = country->getAdjCountries();
				for (auto &iter : adjacentCountries) {
					if (iter->getOwnersId() == m_player->id())
						possibleTargets.push_back(iter);
				}

				if (possibleTargets.size() > 0) {
					int fortifyTargetIndex = rand() % possibleTargets.size();
					int armiesToMove = rand() % (country->getArmyValue() - 1) + 1;
					m_player->moveArmies(country, possibleTargets.at(fortifyTargetIndex), armiesToMove);

					cout << "\nFortification Result\n================\n";
					cout << country->getName() << " has moved " << armiesToMove << " armies successfully.\n";
					cout << country->getName() << " now has: " << country->getArmyValue() << endl;
					cout << possibleTargets.at(fortifyTargetIndex)->getName() << " now has: " << possibleTargets.at(fortifyTargetIndex)->getArmyValue() << endl;
					cout << "The fortification phase is over\n\n";
				}
			}
		}
	}
}

int RandomStrategy::chooseDefensiveDice(shared_ptr<Country>& country)
{
	int armyValue = country->getArmyValue();
	int maxDefDice = (armyValue > 2 ? 2 : armyValue);
	if(maxDefDice > 0)
		return (rand() % maxDefDice) + 1;
	return 0;
}

RandomStrategy::~RandomStrategy()
{
}