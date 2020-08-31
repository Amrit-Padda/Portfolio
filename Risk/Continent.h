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
#include "Country.h"

using namespace std;

// A Continent class for the game Risk
// a continent contains a collection of countries
// a continent contains a bonus point value to be awarded if a player owns all the countries in the continent
// a continent sets an identifier for all the countries it contains, used for the continent subgraph
class Continent {
public:
	Continent(string, vector<shared_ptr<Country>>);
	Continent(Continent *);

	//GETTERS & SETTERS
	vector<shared_ptr<Country>>	getCountries();
	vector<std::string>			getCountriesNames();

	void	setMemberCountries(vector<shared_ptr<Country>>);
	string	getName();
	void	setBonusPoints(int);
	int		getBonusPoints() const;

	bool awardBonus(int) const;

private:

	string						m_name;
	vector<shared_ptr<Country>>	m_countries;
	int							m_bonusPoints;	//The value of the country when a player owns the totality of the countries
	int							m_countryLocalID; //Continent gives its countries an ID

	bool setLocalCountriesID();
	bool setLocalCountryID(Country *c);
};