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
#include "Country.h"

int Country::m_staticGlobalID = 0;

Country::Country(string name)
{
	m_name = name;
	m_occupyingPlayer = nullptr;
	m_globalID = m_staticGlobalID;
	m_staticGlobalID += 1;
	m_localID = -1;
	m_armyValue = 0;
}

Country::Country(Country * c)
{
	m_globalID = c->m_globalID;
	m_localID = c->m_localID;
	m_name = c->getName();
	m_adjCountries = c->getAdjCountries();
	m_armyValue = c->getArmyValue();
	m_occupyingPlayer = c->getOccupyingPlayer();
}

int Country::getCountryId()
{
	return m_globalID;
}

int Country::getLocalID()
{
	return m_localID;
}

string Country::getName()
{
	return m_name;
}

vector<shared_ptr<Country>>& Country::getAdjCountries()
{
	return m_adjCountries;
}

shared_ptr<Player>& Country::getOccupyingPlayer()
{
	return m_occupyingPlayer;
}

unsigned int Country::getOwnersId() const
{
	return m_occupyingPlayer->id();
}

int Country::getArmyValue()
{
	return m_armyValue;
}

void Country::setOccupyingPlayer(shared_ptr<Player> player)
{

	m_occupyingPlayer = player;
	//notify();
}

void Country::setAdjCountries(std::vector<shared_ptr<Country>> & countries)
{
	m_adjCountries = countries;
}

void Country::setArmyValue(int val)
{
	m_armyValue = val;
}

void Country::subFromArmyValue(int val)
{
	m_armyValue -= val;
}

void Country::clear()
{
	m_staticGlobalID = 0;
}

void Country::addToArmyValue(int val)
{
	m_armyValue += val;
}

void Country::setLocalID(int val)
{
	m_localID = val;
}

bool Country::operator==(const int n)
{
	return this->m_globalID == n;
}

bool Country::operator==(std::string &s)
{
	return this->m_name == s;
}
