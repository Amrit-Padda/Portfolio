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

#include "Map.h"

Map::Map()
{
}

Map::Map(vector<vector<string>> countriesInContinent, vector<int> continentValues, vector<vector<string>> adjCountriesList)
{
	createMapAndContinents(countriesInContinent, continentValues, adjCountriesList);
	createAdjacencyList();

	// validate the map by performing a breadth first search on the world graph and continent subgraphs
	validateMap();
}

// create map and continents using the names stored in the parameters
void Map::createMapAndContinents(vector<vector<string>> countriesInContinent, vector<int> continentValues, vector<vector<string>> adjCountriesList)
{
	//Create the continent with all the countries inside
	for (unsigned int i = 0; i < countriesInContinent.size(); i++) {
		vector<shared_ptr<Country>> holdCountriesForCont;

		for (unsigned int j = 1; j < countriesInContinent.at(i).size(); j++) {
			auto country = make_shared<Country>(new Country(countriesInContinent.at(i).at(j)));
			m_countries.push_back(country);
			holdCountriesForCont.push_back(country);
		}

		Continent *c = new Continent(countriesInContinent.at(i).at(0), holdCountriesForCont);
		auto continent = make_shared<Continent>(c);
		continent->setBonusPoints(continentValues.at(i));

		m_continents.push_back(continent);
	}

	//Get the countries and add its adjacent countries to it
	for (unsigned int i = 0; i < adjCountriesList.size(); i++) {
		vector<shared_ptr<Country>> holdAdjCountries;

		for (unsigned int j = 1; j < adjCountriesList.at(i).size(); j++) {
			auto adj = findCountryByName(adjCountriesList.at(i).at(j));
			
			if (adj != NULL) {
				holdAdjCountries.push_back(adj);
			}
		}

		auto country = findCountryByName(adjCountriesList.at(i).at(0));
		if (country != NULL) {
			country->setAdjCountries(holdAdjCountries);
		}
	}
}

void Map::createAdjacencyList()
{
	m_world = make_shared<Graph>(getNumberOfCountries());

	for (unsigned int i = 0; i < m_countries.size(); i++) {

		for (unsigned int j = 0; j < m_countries.at(i)->getAdjCountries().size(); j++) {
			int id = findCountryIndexByName(m_countries.at(i)->getAdjCountries().at(j)->getName());
			m_world->addEdge(i, id);
		}
	}
}

void Map::createContinentSubGraphs()
{
	m_continentsSubGraph = vector<shared_ptr<Graph>>(m_continents.size());

	for (unsigned int i = 0; i < m_continents.size(); i++) {

		int size = m_continents.at(i)->getCountries().size();
		vector<shared_ptr<Country>> currentCountries = m_continents.at(i)->getCountries();

		//Create the graph with the size of number of countries
		auto subGraphElement = make_shared<Graph>(size);
		m_continentsSubGraph.at(i) = subGraphElement;
		vector<int> countryIdList;
		vector<int> localCountryIdList;

		//Loop through every country in this current continent
		for (int j = 0; j < size; j++) {
			countryIdList.push_back(currentCountries.at(j)->getCountryId());
			localCountryIdList.push_back(currentCountries.at(j)->getLocalID());
		}

		for (int j = 0; j < size; j++) {
			for (int k = 0; k < size; k++) {

				//Check if the countries are adjacent to each other through the world adjacency list
				//Then insert its local country ID

				if (m_world->isAdjacentTo(countryIdList.at(j), countryIdList.at(k))) {
					m_continentsSubGraph.at(i)->addEdge(localCountryIdList.at(j), localCountryIdList.at(k));
				}
			}
		}
	}
}

shared_ptr<Country> Map::findCountryByName(string name)
{
	for (unsigned int i = 0; i < m_countries.size(); i++) {
		if (name == m_countries.at(i)->getName()) {
			return m_countries.at(i);
		}
	}
	return NULL;
}

int Map::findCountryIndexByName(string name)
{
	for (unsigned int i = 0; i < m_countries.size(); i++) {
		if (name == m_countries.at(i)->getName()) {
			return i;
		}
	}
	return -1;
}

shared_ptr<Country> Map::findCountryById(int id)
{
	return m_countries.at(id);
}

bool Map::isPlayerOwnsContinent(shared_ptr<Player>p, shared_ptr<Continent>c)
{
	for (shared_ptr<Country> co : c->getCountries()) {
		if (co->getOccupyingPlayer() != p)
			return false;
	}
	return true;
}

bool Map::isCountriesAConnectedGraph(int s) {
	return m_world->BFS(s);
}

bool Map::areContinentSubGraphs()
{
	for (unsigned int i = 0; i < m_continentsSubGraph.size(); i++) {
		if (!m_continentsSubGraph.at(i)->BFS(0))
			return false;
	}
	return true;
}

void Map::validateMap()
{
	if (!isCountriesAConnectedGraph(0)) {
		throw runtime_error("ERROR: The map is not fully connected\n");
	}

	createContinentSubGraphs();
	if (!areContinentSubGraphs()) {
		throw runtime_error("ERROR: One or many continents are not fully connected\n");
	}
}

vector<shared_ptr<Continent>> Map::getContinents()
{
	return m_continents;
}

vector<shared_ptr<Country>>& Map::getCountries()
{
	return m_countries;
}

int Map::getNumberOfCountries()
{
	return m_countries.size();
}

bool Map::exists()
{
	return m_world != NULL;
}

void Map::clear()
{
	Country::clear();
}


void Map::operator=(Map *m)
{
	this->m_continents		= m->m_continents;
	this->m_countries		= m->m_countries;
	this->m_world			= m->m_world;
	this->m_continentsSubGraph = m->m_continentsSubGraph;
}