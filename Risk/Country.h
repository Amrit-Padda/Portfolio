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
#include "Player.h"

using namespace std;

// a Country class in the game Risk
// a country has a collection of pointers to it's adjacent countries
// a country has a pointer to the player that occupies it
// a country has an army value that states how many armies are currently occupying it
// a country has a global ID for the world graph, and a separate local ID for the continent subgraph
class Country {
public:
	Country(string);
	Country(Country *);

	int							getCountryId();
	int							getLocalID();
	string						getName();
	vector<shared_ptr<Country>>&	getAdjCountries();
	shared_ptr<Player>&			getOccupyingPlayer();
	unsigned int				getOwnersId() const;
	int							getArmyValue();

	void setOccupyingPlayer(shared_ptr<Player>);
	void setAdjCountries(vector<shared_ptr<Country>> &);
	void setArmyValue(int);

	void addToArmyValue(int);
	void subFromArmyValue(int);

	static void clear();

	bool operator==(const int);
	bool operator==(string &);
private:

	friend class Continent;

	static int	m_staticGlobalID;
	int			m_globalID; //ID for the world
	int			m_localID;  //ID for the country in a specific continent
	
	string						m_name;
	vector<shared_ptr<Country>>	m_adjCountries;
	int							m_armyValue;
	shared_ptr<Player>			m_occupyingPlayer;
	
	void setLocalID(int);
};