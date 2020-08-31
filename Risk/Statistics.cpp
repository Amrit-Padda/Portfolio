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
#include "Statistics.h"
#include "Game.h"

Statistics::Statistics(shared_ptr<Game> &game) : m_game(game)
{
}

void Statistics::update() {

    if(m_game->gameOver())
        cout << m_game->getActivePlayer()->name() << " won the game!" << endl;
    displayPercentages(m_game->getPlayers());
}


void Statistics::displayPercentages(vector<shared_ptr<Player>> players) {
    for(unsigned int i = 0; i < players.size(); ++i){
		if (!players.at(i)->Eliminated()) {
			double percentage = 0.0;
			int denominator = m_game->getCountries().size();
			if (denominator != 0)
				percentage = (double)players.at(i)->getCountries().size() / denominator;
			cout << "Player " << players.at(i)->name() << " owns " << setprecision(2) << percentage << " of countries" << endl;
		}
    }
}

