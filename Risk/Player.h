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

#ifndef _PLAYER_H
#define _PLAYER_H

#include "stdafx.h"
#include "Dice.h"
#include "Hand.h"
#include "Subject.h"
#include "EnumPhase.h"

using namespace std;

// forward declaration of Country class so Player can use Country objects
class Country;
class Strategy;
// A class for a player in the game Risk
// players have a collection of countries
// players have a hand containing cards
// players have a set of dice to roll during combat
// examples:
//		Player p("John");
//		p.draw(card);
//		int armies = p.exchange();
//		vector<int> results = p.roll(3);
class Player : public Subject {
public:
	Player();
	Player(string &);
	Player(Player *);

	// main player phase functions
	void reinforce(int);
	void attack();
	void fortify();

	// other functions
	void				draw(shared_ptr<Card> &);
	void				showHand();
	shared_ptr<Country>	findCountry(vector<shared_ptr<Country>> &vpc, int id);
	int					chooseDefensiveDice(shared_ptr<Country> &country);
	vector<int>			roll(int);
	vector<double>		getPercentages();
	int					exchange();
	vector <shared_ptr<Card>> returnCardsToDeck();
	bool moveArmies(shared_ptr<Country> src, shared_ptr<Country> target, int toBeMoved);

	// getters
	string							name() const;
	int								id() const;
	vector<shared_ptr<Country>>&	getCountries();
	int								getReinforcements() const;
	static string					getPhaseAsString(PHASE);
	PHASE							getCurrentPhase() const;
	bool							getActive();
	unsigned int					getExchangeResult() const;
	Hand							getHand();
	int								getHandSize() const;
	string							getStrategyName() const;

	bool							Eliminated() const;

	// setters
	void setStrategy(shared_ptr<Strategy> &strategy);
	void setCountries(vector<shared_ptr<Country>> &);
	void setReinforcements(int n);
	void addCountry(shared_ptr<Country> &);
	void addReinforcements(int);
	void setActive(bool active);
	void setPhase(PHASE phase);
	void setEliminated(bool eliminated);
	void setStrategyName(string strategyName);

	void clear();
	void removeCountry(int);

private:
	static unsigned int	m_staticId;
	unsigned int		m_id;
	const string		m_name;
	string				m_strategyName;
	unsigned int		m_reinforcements;
	unsigned int		m_exchangeResult;
	PHASE				m_currentPhase;
	bool				m_activity;
	bool				m_eliminated;

	vector<shared_ptr<Country>>	m_countries;
	Dice						m_dice;
	Hand						m_hand;
	shared_ptr<Strategy>		m_strategy;
};
#endif // !_PLAYER_H
