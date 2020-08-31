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

#pragma once
#include "stdafx.h"
#include "Strategy.h"

class AggressiveStrategy : public Strategy {
public: 
	AggressiveStrategy(shared_ptr<Player> player);
	void reinforce(int continentBonus);
	void attack();
	void fortify();
	int chooseDefensiveDice(shared_ptr<Country> &country);

	~AggressiveStrategy();
private:
	int m_maxAttackingCountryID; 
};