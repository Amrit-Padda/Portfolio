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

#ifndef RISK_STATISTICS_H
#define RISK_STATISTICS_H

#include "stdafx.h"
#include "Observer.h"
#include "Player.h"



class Game;

class Statistics : public Observer{
public:
    Statistics();
    Statistics(shared_ptr<Game> &game);
    void update();

private:
    shared_ptr<Game> m_game;
	vector<shared_ptr<Player>> m_players;
    void displayPercentages(vector<shared_ptr<Player>> players);
};


#endif //RISK_STATISTICS_H
