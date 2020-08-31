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

#include "HumanStrategy.h"
#include "Country.h"

HumanStrategy::HumanStrategy(shared_ptr<Player> player) : Strategy(player)
{
}

void HumanStrategy::reinforce(int continentBonus)
{
	//Set the phase to reinforcement;
	m_player->setPhase(PHASE::Reinforcement);
	m_player->notify();
	
	// award reinforcements equal to # of countries owned / 3 (minimum 3)
	unsigned int reinforcementsGained = m_player->getCountries().size() / 3;
	cout << "Reinforcements received from countries owned: " << reinforcementsGained << endl;

	// check for owning entire continent to award additional reinforcements
	reinforcementsGained += continentBonus;
	cout << "Reinforcements received from continent bonus: " << continentBonus << endl;

	// allow player to exchange cards for armies (5 first time, +5 each time after)
	// force exchange if hand size > 5
	unsigned int exchangeResult = 0;
	m_player->setPhase(PHASE::Exchanging);
	do {
		exchangeResult = m_player->exchange();
		reinforcementsGained += exchangeResult;
	} while (m_player->getHandSize() > 5);

	// award minimum 3 after this
	m_player->setReinforcements( (reinforcementsGained > 3) ? reinforcementsGained : 3 );

	m_player->notify();

	unsigned int countryId = -1;
	unsigned int reinforcementsToAdd = 0;

	m_player->setPhase(PHASE::Reinforcing);
	// Display all countries owned by the player and find the one to reinforce by Id.
	// continue until they have no reinforcements left
	vector<shared_ptr<Country>> countries = m_player->getCountries();
	while (reinforcementsGained > 0) {
		cout << "ID\t# of armies\tName" << endl;
		for (auto &iter : countries) {
			cout << iter->getCountryId() << "\t" << iter->getArmyValue() << "\t\t" << iter->getName() << endl;
		}
		while (cout << "Choose a country (by ID) to add reinforcements to: " && !(cin >> countryId) || countryId < 0) {
			cout << "That is not a valid ID of a country" << endl;
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}

		auto country = m_player->findCountry(countries, countryId);
		if (country) {
			cout << country->getName() << " has " << country->getArmyValue() << " armies" << endl;
			while (cout << "How many reinforcements will you add? (You have " << reinforcementsGained << "). : " && !(cin >> reinforcementsToAdd) || reinforcementsToAdd < 0 || reinforcementsToAdd > reinforcementsGained) {
				cout << "That is not a valid number of reinforcements to add" << endl;
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}
			country->addToArmyValue(reinforcementsToAdd);
			reinforcementsGained -= reinforcementsToAdd;
			m_player->setReinforcements(reinforcementsGained);
			cout << "Added " << reinforcementsToAdd << " armies to " << country->getName() << endl;
		}
		else
			cout << "Country not found!" << endl;
	}
}

void HumanStrategy::attack()
{
	//Set the current phase to be attack
	m_player->setPhase(PHASE::Attack);
	m_player->notify();

	string answer;

	while (cout << "Do you wish to attack? (y, n) " && (!(cin >> answer) || (answer != "y" && answer != "n"))) {
		cout << "Please enter a valid answer (y or n)" << endl;
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	bool attacking = (answer == "y" ? true : false);

	// as long as the Player chooses to continue attacking
	while (attacking) {
		m_player->setPhase(PHASE::Attacking);

		int attackingCountryId = -1;
		int defendingCountryId = -1;

		// a Player can attack if they have a Country with armyValue > 2
		bool canAttack = false;

		// display all countries owned by this player that can attack (if they have at least 2 armies)
		vector<shared_ptr<Country>> countries = m_player->getCountries();
		cout << "ID\t# of armies\tName" << endl;
		for (auto &iter : countries) {
			if (iter->getArmyValue() >= 2) {
				// found at least 1 country that can attack
				canAttack = true;
				cout << iter->getCountryId() << "\t" << iter->getArmyValue() << "\t\t" << iter->getName() << endl;
			}
		}

		cout << endl;

		while (cout << "Choose a country to attack from (by ID): " && (!(cin >> attackingCountryId))) {
			cout << "That is not a positive number" << endl;
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}

		auto attackingCountry = m_player->findCountry(countries, attackingCountryId);
		// If user entered a valid ID for an attacking country
		if (attackingCountry) {
			m_player->notify();

			while (canAttack) {
				// get adjacent countries vector (some of the adjacent countries might be owned by this player)
				// so search vector for countries owned by another player and display them as possible attack targets
				// push the IDs of those countries into a vector of possible targets
				auto adjacentCountries = attackingCountry->getAdjCountries();
				vector<int> possibleTargets;

				cout << "ID\t# of armies\tName" << endl;
				for (auto &iter : adjacentCountries) {
					if (iter->getOwnersId() != m_player->id()) {
						// IDs don't match, possible target found
						cout << iter->getCountryId() << "\t" << iter->getArmyValue() << "\t\t" << iter->getName() << endl;
						possibleTargets.push_back(iter->getCountryId());
					}
				}

				cout << endl;

				if (!possibleTargets.empty()) {
					// let player choose an attack target, the ID they choose must exist in the possibleTargets vector
					while (cout << "Choose a target of your attack (by ID): "
						&& (!(cin >> defendingCountryId)
							|| defendingCountryId < 0
							|| find(possibleTargets.begin(), possibleTargets.end(), defendingCountryId) == possibleTargets.end())) {
						cout << "That is not a number or possible attack target" << endl;
						cin.clear();
						cin.ignore(numeric_limits<streamsize>::max(), '\n');
					}

					auto defendingCountry = m_player->findCountry(adjacentCountries, defendingCountryId);
					// if user entered a valid ID for a defending country
					if (defendingCountry) {
						// determing how many dice the attacker can roll
						int atkArmyValue = attackingCountry->getArmyValue();
						int maxAtkDice = (atkArmyValue > 3 ? 3 : (atkArmyValue - 1));
						int atkDice = 0;

						// ask attacker how many to roll
						while (cout << m_player->name() << ", you can roll " << maxAtkDice << " attack dice, choose how many: " && (!(cin >> atkDice) || atkDice < 1 || atkDice > maxAtkDice)) {
							cout << "That is not a number between 1 and " << maxAtkDice << endl;
							cin.clear();
							cin.ignore(numeric_limits<streamsize>::max(), '\n');
						}

						// determine how many dice the defender can roll
						shared_ptr<Player> defPlayer = defendingCountry->getOccupyingPlayer();
						int defArmyValue = defendingCountry->getArmyValue();
						int defDice = defPlayer->chooseDefensiveDice(defendingCountry);

						m_player->setPhase(PHASE::BattleResults);
						m_player->notify();

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
								cout << defPlayer->name() << " lost 1 army" << endl;
								defArmyValue--;
								if (defArmyValue <= 0) {
									// defender has no armies left
									conquered = true;

									// pop 1 element from possible targets (doesn't matter which).
									// we want to force the player to choose another country to attack from if there are no adjacent countries to attack.
									possibleTargets.pop_back();

									cout << defendingCountry->getName() << " has been conquered!" << endl;
									cout << "Switching ownership from " << defendingCountry->getOccupyingPlayer()->name() << " to " << m_player->name() << endl << endl;

									// prompt user for # of armies to move over
									int armiesToMove = 0;

									while (cout << "How many armies do you want to move to " << defendingCountry->getName() << "? (1 to " << (atkArmyValue - 1) << ")"
										&& (!(cin >> armiesToMove) || armiesToMove < 1 || armiesToMove >(atkArmyValue - 1))) {
										cout << "That is not a valid value between 1 and " << (atkArmyValue - 1) << endl;
										cin.clear();
										cin.ignore(numeric_limits<streamsize>::max(), '\n');
									}
									defendingCountry->setArmyValue(armiesToMove);
									attackingCountry->setArmyValue(atkArmyValue - armiesToMove);

									// switch ownership
									// we use the erase / remove idiom to remove it from the defending player's vector of countries (in Player::removeCountry(int))
									// then set that countries owner to this player, and add it to this player's vector of countries
									defPlayer->removeCountry(defendingCountryId);
									defendingCountry->setOccupyingPlayer(m_player);
									m_player->addCountry(defendingCountry);
									break;
								}
							}
							// defender wins on tie or better
							else {
								cout << m_player->name() << " lost 1 army" << endl;
								atkArmyValue--;
							}
						} // for

						cout << endl;

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
							while (cout << "Do you wish to attack again from the same country? (y, n) " && (!(cin >> answer) || (answer != "y" && answer != "n"))) {
								cout << "Please enter a valid answer (y or n)" << endl;
								cin.clear();
								cin.ignore(numeric_limits<streamsize>::max(), '\n');
							}
							if (answer == "n" || possibleTargets.empty()) {
								canAttack = false;
								if (possibleTargets.empty())
									cout << "There are no targets adjacent to that country!" << endl;
							}
						}
						else
							canAttack = false;
					} // if(defendingCountry)
					else {
						cout << "Could not find the defending country!" << endl;
					}
				} // if(!possibleTargets.empty()
				else {
					cout << "There are no targets adjacent to that country!" << endl;
					canAttack = false;
				}
				m_player->setPhase(PHASE::Attacking);
				m_player->notify();
			} // while(canAttack)
		} // if(attackingCountry)
		else
			cout << "Could not find the attacking country!" << endl;

		// let player attack with another country until they decide to stop
		m_player->setPhase(PHASE::Attacking);
		m_player->notify();
		while (cout << "Do you wish to continue attacking with other countries? (y, n) " && (!(cin >> answer) || (answer != "y" && answer != "n"))) {
			cout << "Please enter a valid answer (y or n)" << endl;
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
		attacking = (answer == "y" ? true : false);
	} // while(attacking)
}

void HumanStrategy::fortify()
{
	//Set the current phase to fortification
	m_player->setPhase(PHASE::Fortification);
	m_player->notify();

	string answer;
	vector<shared_ptr<Country>> countries = m_player->getCountries();

	while (cout << "Do you wish to fortify? (y, n) " && (!(cin >> answer) || (answer != "y" && answer != "n"))) {
		cout << "Please enter a valid answer (y or n)" << endl;
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	bool fortifying = (answer == "y" ? true : false);

	// as long as the Player chooses to continue attacking
	while (fortifying) {

		int sourceCountryId = -1;
		int targetCountryId = -1;

		// a Player can attack if they have a Country with armyValue > 2
		bool canFortify = false;

		// display all countries owned by this player that can attack (if they have at least 2 armies)
		cout << "ID\t# of armies\tName" << endl;
		for (auto &iter : countries) {
			if (iter->getArmyValue() >= 2) {
				// found at least 1 country that can fortify
				canFortify = true;
				cout << iter->getCountryId() << "\t" << iter->getArmyValue() << "\t\t" << iter->getName() << endl;
			}
		}

		while (cout << "Choose your source country (by ID): " && (!(cin >> sourceCountryId))) {
			cout << "That is not a positive number" << endl;
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}

		cout << "=======================================\n";
		auto sourceCountry = m_player->findCountry(countries, sourceCountryId);

		if (sourceCountry) {

			while (canFortify) {
				// get adjacent countries vector (some of the adjacent countries might be owned by this player)
				// so search vector for countries owned by this player and display them as possible target countries
				// push the IDs of those countries into a vector of possible targets
				auto adjacentCountries = sourceCountry->getAdjCountries();
				vector<int> possibleTargets;

				cout << "ID\t# of armies\tName" << endl;
				for (auto &iter : adjacentCountries) {
					if (iter->getOwnersId() == m_player->id()) {
						// IDs don't match, possible target found
						cout << iter->getCountryId() << "\t" << iter->getArmyValue() << "\t\t" << iter->getName() << endl;
						possibleTargets.push_back(iter->getCountryId());
					}
				}

				// let player choose a target country, the ID they choose must exist in the possibleTargets vector
				while (cout << "Choose a target country (by ID): "
					&& (!(cin >> targetCountryId)
						|| targetCountryId < 0
						|| find(possibleTargets.begin(), possibleTargets.end(), targetCountryId) == possibleTargets.end())) {
					cout << "That is not a number or possible target country" << endl;
					cin.clear();
					cin.ignore(numeric_limits<streamsize>::max(), '\n');
				}

				auto targetCountry = m_player->findCountry(adjacentCountries, targetCountryId);

				int selectedArmy = -1;

				if (targetCountry) {
					while (cout << sourceCountry->getName() << " has " << sourceCountry->getArmyValue() - 1 << " available armies.\nHow many do you wish to move: "
						&& (!(cin >> selectedArmy)
							|| selectedArmy < 1
							|| selectedArmy >(sourceCountry->getArmyValue() - 1))) {
						cout << "That is not a valid number of armies" << endl;
						cin.clear();
						cin.ignore(numeric_limits<streamsize>::max(), '\n');
					}

					//Move the selected amount of armies from sourceCountry to targetCountry
					m_player->moveArmies(sourceCountry, targetCountry, selectedArmy);

					cout << "\nFortification Result\n================\n";
					cout << sourceCountry->getName() << " has moved " << selectedArmy << " armies successfully.\n";
					cout << sourceCountry->getName() << " now has: " << sourceCountry->getArmyValue() << endl;
					cout << targetCountry->getName() << " now has: " << targetCountry->getArmyValue() << endl;
					cout << "The fortification phase is over\n\n";

					//done fortifying
					fortifying = false;
					canFortify = false;
					break;
				}
			}
		}
		else {
			cout << "The source country was invalid.\n";
		}
	}
}

int HumanStrategy::chooseDefensiveDice(shared_ptr<Country>& country)
{
	int defArmyValue = country->getArmyValue();
	int maxDefDice = (defArmyValue > 2 ? 2 : defArmyValue);
	int defDice = 0;

	// ask defender how many to roll
	while (cout << m_player->name() << ", you can roll " << maxDefDice << " defend dice, choose how many: " && (!(cin >> defDice) || defDice < 1 || defDice > maxDefDice)) {
		cout << "That is not a number between 1 and " << maxDefDice << endl;
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}

	return defDice;
}

HumanStrategy::~HumanStrategy()
{
}
