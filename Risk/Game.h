/*
	COMP 345 D Fall 2018
	Assignment 3
	Written by Team #9:
		Amritpal Padda	40042768
		Francois Roy	40065497
		Jason Brennan	27793928
		Olivier Hébert	40051654

	Submitted on November 18, 2018
*/

#ifndef _GAME_H
#define _GAME_H

#include "stdafx.h"
#include "Continent.h"
#include "Map.h"
#include "MapLoader.h"
#include "Player.h"
#include "Deck.h"
#include "HumanStrategy.h"
#include "AggressiveStrategy.h"
#include "PassiveStrategy.h"
#include "CheaterStrategy.h"
#include "RandomStrategy.h"
#include "Observer.h"
#include "EnumPhase.h"
#include "Statistics.h"

using namespace std;

// The Game class manages the game state and contains all objects required by the game
// Game uses a MapLoader to load a map into a Map object
// Game has a collection of Players, it will kwwp track of the current Player's turn and execute their methods accordingly
// Game has a deck of Cards
// example:
//		Game g("World.map", 4); // 4 players
//		g.run();
//		g.drawCard() {
//			m_players[m_activePlayer]->draw(m_deck.draw());
//		}
class Game : public Observer {
public:
	Game();
	Game(Game* g);
	~Game();

	bool initialize();
	bool initialize(int index);
	void initializeTournament();
	void run();
	void runTournament();
	void clear();
	void update();

	// Methods for demo of assignment 2
	void setPlayers(vector<shared_ptr<Player>> players);
	vector<shared_ptr<Player>>& getPlayers();
	shared_ptr<Player>& getActivePlayer();

	vector<shared_ptr<Country>> getCountries();
	template<bool RECURSIVE>
	void listMapFiles(filesystem::path, regex);

	// Methods for demo of assignment 3
	void setViewHeader(const string &str);
	void setViewInformation(const string &str);
	string getViewHeader() const;
	string getViewInformation() const;
	void displayView() const;

	void distributeCountries();
	void distributeInitialArmies();
	void changeActivePlayer() const;
	void drawCard();
	void putBackCards(vector<shared_ptr<Card>>);

	// Methods for demo of assignment 4
	unsigned int getNumOfMaps() const;
	unsigned int getNumOfGamesPerMap() const;
	unsigned int getMaxTurns() const;
	unsigned int getTurnNumber() const;
	void setMaxTurns(int maxTurns);
	void setTurnNumber(int turnNumber);

	void removeConqueredPlayer();
	void checkWinCondition();
	bool gameOver() const;
	void declareWinner() const;

private:
	MapLoader						m_mapLoader;
	Map								m_map;
	vector<string>					m_mapNames;
	vector<shared_ptr<Player>>		m_players;
	Deck							m_deck;
	unsigned int					m_numOfGamesPerMap;
	const unsigned int				m_ARMY_DISTRIBUTION_VALUE = 70;
	unsigned int					m_numPlayers;
	unsigned int					m_activePlayer;
	bool							m_gameOver;
	unsigned int					m_previousPlayer;
	unsigned int					m_maxTurns;
	unsigned int					m_turnNumber;

	string							m_winnersName;
	string							m_viewHeader;
	string							m_viewInformation;
	vector<vector<string>>			m_resultsTable;

	Observer*                      m_statistics;
};

template<bool RECURSIVE>
void Game::listMapFiles(filesystem::path dir, regex pattern)
{
	std::vector<filesystem::path> result;

	using iterator = std::conditional< RECURSIVE,
		filesystem::recursive_directory_iterator, filesystem::directory_iterator >::type;

	const iterator end;
	for (iterator iter{ dir }; iter != end; ++iter)
	{
		const std::string extension = iter->path().extension().string();
		if (filesystem::is_regular_file(*iter) && std::regex_match(extension, pattern))
			result.push_back(*iter);
	}

	for (auto &iter : result)
		cout << iter << endl;
}

#endif // !_GAME_H
