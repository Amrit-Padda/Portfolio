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

#include "Continent.h"

Continent::Continent(string name, vector<shared_ptr<Country>> countries)
{
	m_name = name;

	m_countryLocalID = 0;
	for (unsigned int i = 0; i < countries.size(); i++) {
		m_countries.push_back(countries.at(i));
	}

	//Now that a set of countries is owned by a continent, declare its local id within the continent
	if (!setLocalCountriesID()) {
		cout << "ERROR\nOne or many more countries belong to more than one continent\n";
		exit(4);
	}
}

Continent::Continent(Continent *c) :
	m_name(c->m_name), m_countries(c->m_countries), m_bonusPoints(c->m_bonusPoints), m_countryLocalID(c->m_countryLocalID)
{
}

void Continent::setMemberCountries(vector<shared_ptr<Country>> countries)
{
	m_countries = countries;
}

vector<shared_ptr<Country>> Continent::getCountries()
{
	return m_countries;
}

vector<string> Continent::getCountriesNames()
{
	vector<string> temp;
	for (unsigned int i = 0; i < m_countries.size(); i++) {
		temp.push_back(m_countries.at(i)->getName());
	}
	return temp;
}

string Continent::getName()
{
	return m_name;
}

void Continent::setBonusPoints(int bonus)
{
	m_bonusPoints = bonus;
}

int Continent::getBonusPoints() const
{
	return m_bonusPoints;
}

// if the id of each player occupying the countries in this continent matches the parameter id,
// then that player owns this continent and bonus armies can be awarded
bool Continent::awardBonus(int id) const
{
	for (auto &itr : m_countries) {
		if (itr->getOwnersId() != id)
			return false;
	}
	return true;
}

bool Continent::setLocalCountriesID()
{
	for (unsigned int i = 0; i < m_countries.size(); i++) {

		//The country has already been initialized
		//	Therefore the country is not unique to a continent : return false
		if (m_countries[i]->getLocalID() != -1) {
			return false;
		}
		else {
			m_countries[i]->setLocalID(m_countryLocalID++);
		}
	}
	return true;
}
bool Continent::setLocalCountryID(Country * c)
{
	//The country has already been initialized
	//	Therefore the country is not unique to a continent : return false
	if (c->getLocalID() != -1) {
		return false;
	}
	else {
		c->setLocalID(m_countryLocalID++);
		return true;
	}
}