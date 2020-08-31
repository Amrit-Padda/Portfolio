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

#include "Strategy.h"
#include "Player.h"
#include "Country.h"


using namespace std;
using std::string;

Strategy::Strategy()
{
}

Strategy::Strategy(shared_ptr<Player> &player) : m_player(player)
{
}

Strategy::~Strategy() {

}
