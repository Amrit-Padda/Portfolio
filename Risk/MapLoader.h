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
#include "Map.h"

using namespace std;

// A class to load a .map conquest file and instantiate a Map object
// map files are parsed line by line, extracting continent names, country names, adjacencies, and continent bonus point values
// the MapLoader object takes the extracted strings and creates collections of continent names, country names and adjacencies
// the MapLoader passes these collections to the Map constructor
class MapLoader {
public:
	MapLoader();
	MapLoader(string);

	Map *	getMap();
	string	getFileName();
	void	setFileName(string);

	void loadMap();
	void clear();

	~MapLoader();
private:
	string	m_fileName;
	Map *	m_map;
};