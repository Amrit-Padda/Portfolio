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

#include "MapLoader.h"

MapLoader::MapLoader()
{
	m_map = NULL;
}

MapLoader::MapLoader(string filename)
{
	m_fileName = filename;
}

Map * MapLoader::getMap()
{
	return m_map;
}

string MapLoader::getFileName()
{
	return m_fileName;
}

void MapLoader::setFileName(string s)
{
	m_fileName = s;
}

// parse a .map conquest file and instantiate a member map object
void MapLoader::loadMap()
{
	string continentDelimeter = "[Continents]";
	string countryDelimeter = "[Territories]";
	ifstream inFile;
	string line;

	vector<string> countries;
	vector<vector<string>> adjCountries;
	vector<vector<string>> countriesInContinent;
	vector<int> continentsValue;

	string prevContinent = "";

	inFile.open(m_fileName);
	if (!inFile)
		throw runtime_error("Could not open file, check spelling or file location");

	//Read the file line by line
	while (getline(inFile, line)) {

		//Search for [Continents]
		string curData;

		//Found [Continents]
		if (line == continentDelimeter) {

			while (getline(inFile, curData)) {

				//Read until blank line
				if (curData.empty()) break;
				else {
					//Store the continent value 
					vector<string> c;
					c.push_back(curData.substr(0, curData.find('=')));
					countriesInContinent.push_back(c);

					continentsValue.push_back(std::stoi(curData.substr(curData.find('=') + 1, curData.size() - 1)));
				}
			}
		}
		else if (line == countryDelimeter) {

			// The file is is divided under this format:
			// Territory name, X, Y, Continent Name, <Adjacent countries>	

			while (getline(inFile, curData)) {

				//Found the country
				string currCountry = curData.substr(0, curData.find(','));
				if (currCountry != "") {
					for (unsigned int i = 0; i < countries.size(); i++) {
						if (currCountry == countries.at(i)) {
							throw runtime_error("Could not create map, duplicate countries found in map file: (" + currCountry + ", " + countries.at(i) + ")");
						}
					}
					countries.push_back(currCountry);

					vector<string> currentAdjCountries;

					currentAdjCountries.push_back(currCountry);

					int occurence = 0;

					//Cut the country name from the string
					curData = curData.substr(curData.find(',') + 1);

					//Read the adjacent countries and continent where the country belongs
					while (curData.find(',') != string::npos) {

						//Cut the X,Y coordinate location from the string - not useful for now ****
						if (occurence < 2) {
							curData = curData.substr(curData.find(',') + 1);
							occurence++;
						}
						//Found the continent 
						else if (occurence == 2) {
							string continent = curData.substr(0, curData.find(','));
							curData = curData.substr(curData.find(',') + 1);

							//Find the correct continent, then add the country to it
							for (unsigned int i = 0; i < countriesInContinent.size(); i++) {
								if (*countriesInContinent.at(i).data() == continent) {
									countriesInContinent[i].push_back(currCountry);
									break;
								}
							}
							occurence++;
						}
						//Adjacent countries
						else {
							//Add the adjacent countries to the list
							currentAdjCountries.push_back(curData.substr(0, curData.find(',')));
							curData = curData.substr(curData.find(',') + 1);
						}
					}
					//Reader skipped last country due to no ',' found at after the last country
					currentAdjCountries.push_back(curData.substr(0, curData.find(',')));
					adjCountries.push_back(currentAdjCountries);
				}
			}
		}
	}
	inFile.close();
	if (!countriesInContinent.empty() && !continentsValue.empty() && !adjCountries.empty())
		m_map = new Map(countriesInContinent, continentsValue, adjCountries);
	else
		throw runtime_error("Error: The map could not be created");
}

void MapLoader::clear()
{
	if (m_map) {
		delete m_map;
		m_map = NULL;
	}
}

MapLoader::~MapLoader()
{
	delete m_map;
}
