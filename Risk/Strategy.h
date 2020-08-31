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

#ifndef _STRATEGY_H
#define _STRATEGY_H

#include "Player.h"

class Strategy {
public:
	Strategy();
	Strategy(shared_ptr<Player> &player);
	//main functions
	virtual void reinforce(int continentBonus) = 0;
	virtual void attack() = 0;
	virtual void fortify() = 0;

	virtual int chooseDefensiveDice(shared_ptr<Country> &country) = 0;

	virtual ~Strategy() = 0;

protected:
	shared_ptr<Player> m_player;
};
#endif //!_STRATEGY_H
