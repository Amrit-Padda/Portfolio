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
#include "Continent.h"
#include "Graph.h"

using namespace std;

// A class to represent a world map in the game Risk
// a map contains a collection of continents, and a collection of countries
// a map contains a Graph object to represent the game world
// a map also contains a vector of Graphs to represent continent subgraphs
// the class accepts .map conquest files, parsed by a MapLoader object
// the Map constructor accepts collections of continents, countries and adjacencies in string format, created by the MapLoader
// the Map class uses these parameters to create Continent and Country objects, and the adjacency list
// a map contains methods to validate the integrity of the map, by calling a breadth first search on it's graph and subgraphs
class Map {
public:
	Map();
	Map(vector<vector<string>>, vector<int>, vector<vector<string>>);

	vector<shared_ptr<Continent>>	getContinents();
	vector<shared_ptr<Country>>&	getCountries();
	int								getNumberOfCountries();

	bool isPlayerOwnsContinent(shared_ptr<Player>p, shared_ptr<Continent>c);

	bool exists();
	void clear();
	void operator=(Map *);

private:
	vector<shared_ptr<Continent>>	m_continents;
	vector<shared_ptr<Country>>		m_countries;

	//Adjacency list of countries
	shared_ptr<Graph>			m_world;
	vector<shared_ptr<Graph>>	m_continentsSubGraph;

	void createMapAndContinents(vector<vector<string>>, vector<int>, vector<vector<string>>);
	void createAdjacencyList();
	void createContinentSubGraphs();

	shared_ptr<Country>	findCountryByName(string);
	int					findCountryIndexByName(string);
	shared_ptr<Country>	findCountryById(int);

	bool isCountriesAConnectedGraph(int);
	bool areContinentSubGraphs();
	void validateMap();
};