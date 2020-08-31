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
#include "Game.h"


Game::Game() :
	m_numOfGamesPerMap(0),
	m_numPlayers(0),
	m_activePlayer(0),
	m_gameOver(false),
	m_previousPlayer(0),
	m_maxTurns(0),
	m_turnNumber(0),
	m_winnersName(""),
	m_viewHeader(""),
	m_viewInformation(""),
	m_resultsTable(0),
	m_statistics(nullptr)
{
	srand((unsigned int)time(nullptr));	//creating a new random seed
}

Game::Game(Game* g) :
m_players(g->m_players),
m_numPlayers(g->m_numPlayers)
{
}

Game::~Game()
{
}

// initialize function for 1 game, user will choose map here
bool Game::initialize()
{
	// clear the game state, fresh start for new game
	clear();

	cout << "---------------------------------------------------" << endl;
	cout << "----------------GAME INITIALIZATION----------------" << endl;
	cout << "---------------------------------------------------" << endl << endl;

	// display .map files to the user, let them choose a map to load
	cout << "Choose a map to load:" << endl << endl;
	listMapFiles<false>("", regex("\\.(?:map)"));
	cout << endl;

	string mapName;
	cout << "Enter map file name: ";
	cin >> mapName;

	m_mapLoader.setFileName(mapName);

	try {
		// a map needs to load succesfully before we can set up anything else
		m_mapLoader.loadMap();
		m_map = m_mapLoader.getMap();

		while (cout << "Enter number of players (2 - 6): " && (!(cin >> m_numPlayers) || m_numPlayers < 2 || m_numPlayers > 6)) {
			cout << "That is not a number between 2 and 6" << endl;
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}

		// store players in temp vector, will randomize order and push to actual vector later
		vector<shared_ptr<Player>> tempPlayers;
		for (unsigned int i = 0; i < m_numPlayers; i++) {
			string name = "";
			while (cout << "Enter player name: " && (!(cin >> name) || name == "")) {
				cout << "Invalid / empty name" << endl;
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}

			int strategyChoice = 0;
			while (cout << "Choose a strategy for this player (1 = Human, 2 = Aggressive, 3 = Passive, 4 = Cheater, 5 = Random): " && (!(cin >> strategyChoice) || strategyChoice < 1 || strategyChoice > 5)) {
				cout << "That is not a choice between 1 and 5" << endl;
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}

			auto player = make_shared<Player>(new Player(name));
			shared_ptr<Strategy> strategy;
			switch (strategyChoice) {
			case 1:
				strategy = make_shared<HumanStrategy>(player);
				cout << "Human Player " << player->name() << " created" << endl;
				break;
			case 2:
				strategy = make_shared<AggressiveStrategy>(player);
				cout << "Aggressive Player " << player->name() << " created" << endl;
				break;
			case 3:
				strategy = make_shared<PassiveStrategy>(player);
				cout << "Passive " << player->name() << " created" << endl;
				break;
			case 4:
				strategy = make_shared<CheaterStrategy>(player);
				cout << "Cheater " << player->name() << " created" << endl;
				break;
			case 5:
				strategy = make_shared<RandomStrategy>(player);
				cout << "Random " << player->name() << " created" << endl;
				break;
			}
			player->setStrategy(strategy);
			player->attach(this);
			tempPlayers.push_back(player);
		}

		// User chooses 10 - 50 maximum number of turns per game
		while (cout << "How many turns until a draw is called? (10 - 50) " && (!(cin >> m_maxTurns) || m_maxTurns < 10 || m_maxTurns > 50)) {
			cout << "That is not a choice between 10 and 50" << endl;
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}

		// The order of players is randomized
		for (unsigned int i = 0; i < m_numPlayers; i++) {
			int index = rand() % tempPlayers.size();
			m_players.push_back(tempPlayers[index]);
			tempPlayers.erase(tempPlayers.begin() + index);
		}

		distributeCountries();
		distributeInitialArmies();
		m_deck.init(m_map.getNumberOfCountries());

		//First player in the list is now starting
		m_players.at(m_activePlayer)->setActive(true);
	}
	catch (runtime_error e) {
		cerr << e.what() << endl;
		return false;
	}
	catch (out_of_range e) {
		cerr << "The program encountered an out of bounds error while verifying the map. Check your map file and try again." << endl;
		return false;
	}
	return true;
}

// initialize a particular game in a tournament using a supplied map at mapIndex
bool Game::initialize(int mapIndex)
{
	// clear the game state, fresh start for new game
	clear();

	m_mapLoader.setFileName(m_mapNames[mapIndex]);

	try {
		// a map needs to load succesfully before we can set up anything else
		m_mapLoader.loadMap();
		m_map = m_mapLoader.getMap();
	}
	catch (runtime_error e) {
		cerr << e.what() << endl;
		return false;
	}
	catch (out_of_range e) {
		cerr << "The program encountered an out of bounds error while verifying the map. Check your map file and try again." << endl;
		return false;
	}

	distributeCountries();
	distributeInitialArmies();
	m_deck.init(m_map.getNumberOfCountries());

	//First player in the list is now starting
	m_players.at(m_activePlayer)->setActive(true);

	return true;
}

void Game::initializeTournament()
{
	// User chooses 1 - 5 different maps
	// store map names in m_mapNames
	int numOfMaps;
	while (cout << "How many maps do you want to play on? (1 - 5) " && (!(cin >> numOfMaps) || numOfMaps < 1 || numOfMaps > 5)) {
		cout << "That is not a choice between 1 and 5" << endl;
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}

	for (int i = 0; i < numOfMaps; i++) {
		// display .map files to the user, let them choose a map to load into m_mapNames
		cout << "Choose a map to load: " << endl << endl;
		listMapFiles<true>("", regex("\\.(?:map)"));
		cout << endl;

		// User inputs the map names, if they are invalid they will be verified and replaced in runTournament() when the game tries to find it and load
		string mapName;
		cout << "Enter map file name: ";
		cin >> mapName;
		m_mapNames.push_back(mapName);
		cout << mapName << " added to tournament queue" << endl << endl;
	}

	// User chooses 2 - 4 different computer players
	// store players in m_players
	while (cout << "Enter number of players (2 - 4): " && (!(cin >> m_numPlayers) || m_numPlayers < 2 || m_numPlayers > 4)) {
		cout << "That is not a number between 2 and 4" << endl;
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}

	// store players in temp vector, will randomize order and push to actual vector later
	vector<shared_ptr<Player>> tempPlayers;
	for (unsigned int i = 0; i < m_numPlayers; i++) {
		string name = "";
		while (cout << "Enter player name: " && (!(cin >> name) || name == "")) {
			cout << "Invalid / empty name" << endl;
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}

		int strategyChoice = 0;
		while (cout << "Choose a strategy for this player (1 = Aggressive, 2 = Passive, 3 = Cheater, 4 = Random): " && (!(cin >> strategyChoice) || strategyChoice < 1 || strategyChoice > 4)) {
			cout << "That is not a choice between 1 and 4" << endl;
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}

		auto player = make_shared<Player>(new Player(name));
		shared_ptr<Strategy> strategy;
		switch (strategyChoice) {
		case 1:
			strategy = make_shared<AggressiveStrategy>(player);
			player->setStrategyName("Aggressive");
			cout << "Aggressive Player " << player->name() << " created" << endl;
			break;
		case 2:
			strategy = make_shared<PassiveStrategy>(player);
			player->setStrategyName("Passive");
			cout << "Passive " << player->name() << " created" << endl;
			break;
		case 3:
			strategy = make_shared<CheaterStrategy>(player);
			player->setStrategyName("Cheater");
			cout << "Cheater " << player->name() << " created" << endl;
			break;
		case 4:
			strategy = make_shared<RandomStrategy>(player);
			player->setStrategyName("Random");
			cout << "Random " << player->name() << " created" << endl;
			break;
		}
		player->setStrategy(strategy);
		tempPlayers.push_back(player);
	}

	// The order of players is randomized
	// It will randomize once and use the same order for each game played
	for (unsigned int i = 0; i < m_numPlayers; i++) {
		int index = rand() % tempPlayers.size();
		m_players.push_back(tempPlayers.at(index));
		tempPlayers.erase(tempPlayers.begin() + index);
	}

	// User chooses 1 - 5 games to be player per map
	while (cout << "How many games do you want to play per map? (1 - 5) " && (!(cin >> m_numOfGamesPerMap) || m_numOfGamesPerMap < 1 || m_numOfGamesPerMap > 5)) {
		cout << "That is not a choice between 1 and 5" << endl;
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}

	// User chooses 10 - 50 maximum number of turns per game
	while (cout << "How many turns until a draw is called? (10 - 50) " && (!(cin >> m_maxTurns) || m_maxTurns < 10 || m_maxTurns > 50)) {
		cout << "That is not a choice between 10 and 50" << endl;
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
}

void Game::run()
{
	while (!gameOver()) {
		if (!m_players[m_activePlayer]->Eliminated()) {
			cout << endl << m_players[m_activePlayer]->name() << "'s turn" << endl;

			// active player draws a card
			auto c = make_shared<Card>(m_deck.draw().get());
			m_players[m_activePlayer]->draw(c);

			// put exchanged cards back in deck
			putBackCards(m_players[m_activePlayer]->returnCardsToDeck());

			// active player reinforces
			int continentBonus = 0;
			for (auto &iter : m_map.getContinents())
				if (iter->awardBonus(m_activePlayer))
					continentBonus += iter->getBonusPoints();
			m_players[m_activePlayer]->reinforce(continentBonus);

			// active player attacks
			m_players[m_activePlayer]->attack();

			// check m_players and remove a player if they have no countries left
			removeConqueredPlayer();

			// check if there is a winner - we check before fortification (once a player has conquered all countries there is no need to fortify)
			checkWinCondition();
			if (m_gameOver)
				return;

			// active player fortifies
			m_players[m_activePlayer]->fortify();
		}

		if (++m_turnNumber >= m_maxTurns) {
			m_winnersName = "Draw";
			m_gameOver = true;
			return;
		}

		// change active player, reset to 0 if at the end
		m_previousPlayer = m_activePlayer;

		// current player is now inactive
		getPlayers().at(m_activePlayer)->setActive(false);

		if (m_numPlayers > 0)
			m_activePlayer = (m_activePlayer + 1) % m_numPlayers;

		//new player is now active
		getPlayers().at(m_activePlayer)->setActive(true);

		//int x = 0;
		//cin >> x;
	}
}

void Game::runTournament()
{
	// create table header
	m_resultsTable.push_back(vector<string>(m_numOfGamesPerMap + 1));
	m_resultsTable.at(0).at(0) = "";
	for (int i = 0; i < m_numOfGamesPerMap; i++)
		m_resultsTable.at(0).at(i + 1) = ("Game " + to_string(i + 1));

	for (int mapIter = 0; mapIter < m_mapNames.size(); mapIter++) {
		m_resultsTable.push_back(vector<string>(m_numOfGamesPerMap + 1));

		for (int gameIter = 0; gameIter < m_numOfGamesPerMap; gameIter++) {
			if (initialize(mapIter)) {
				// run the game
				cout << "****************************" << endl;
				cout << "Game " << to_string(gameIter + 1) << " on " << m_mapNames.at(mapIter) << " started" << endl;
				cout << "****************************" << endl << endl;

				run();

				cout << "****************************" << endl;
				cout << "Game " << to_string(gameIter + 1) << " on " << m_mapNames.at(mapIter) << " ended" << endl;
				cout << "****************************" << endl;

				// game over, store winner's name in results table
				m_resultsTable.at(mapIter + 1).at(gameIter + 1) = m_winnersName;
			}
			else {
				// the map could not be loaded, let user specify new map here
				// decrement gameIter and try again
				cout << m_mapNames.at(mapIter) << " could not be loaded in (check spelling or file location)" << endl;
				
				cout << "Choose a map to load: " << endl << endl;
				listMapFiles<true>("", regex("\\.(?:map)"));
				cout << endl;

				cout << "Enter map file name: ";
				string mapName;
				cin >> mapName; 
				m_mapNames.at(mapIter) = mapName;
				cout << mapName << " inserted to tournament queue" << endl << endl;
				gameIter--;
			}
		}
	}

	for (int mapIter = 0; mapIter < m_mapNames.size(); mapIter++) {
		m_resultsTable.at(mapIter + 1).at(0) = m_mapNames.at(mapIter);
	}

	// Display results of tournament
	cout << endl;
	cout << "*********************" << endl;
	cout << "RESULTS OF TOURNAMENT" << endl;
	cout << "*********************" << endl << endl;
	cout << "Maps: ";
	for (int maps = 0; maps < m_mapNames.size(); maps++)
		cout << m_mapNames.at(maps) << ", ";
	cout << endl;

	cout << "Players: ";
	for (int player = 0; player < m_players.size(); player++)
		cout << m_players.at(player)->getStrategyName() << ", ";
	cout << endl;

	cout << "Games: " << m_numOfGamesPerMap << endl;
	cout << "Turns: " << m_maxTurns << endl;

	for (int row = 0; row < m_resultsTable.size(); row++) {
		for (int column = 0; column < m_resultsTable.at(row).size(); column++) {
			if (row == 0 && column == 0)
				cout << m_resultsTable.at(row).at(column) << "\t\t\t";
			else
				cout << m_resultsTable.at(row).at(column) << "\t\t";
		}
		cout << endl;
	}

	// pause here, program will end after
	int n = 0;
	cin >> n;
}

// clear the game state (map, players, deck) and prepare for a new game session
void Game::clear()
{
	for (auto &player : m_players) {
		player->clear();
	}
	m_mapLoader.clear();
	m_map.clear();
	m_deck.clear();
	m_activePlayer = 0;
	m_numPlayers = m_players.size();
	m_gameOver = false;
	m_previousPlayer = 0;
	m_viewHeader = "";
	m_viewInformation = "";
	m_turnNumber = 0;
	if (m_statistics != nullptr) {
		delete m_statistics;
		m_statistics = nullptr;
	}

}

void Game::setPlayers(vector<shared_ptr<Player>> players)
{
	m_players = players;
}

void Game::update()
{
	auto p = getActivePlayer();
	PHASE phase = p->getCurrentPhase();
	string phaseString = Player::getPhaseAsString(phase);
	string name = p->name();

	switch (phase) {
	case PHASE::Attack:
	case PHASE::Fortification:
	case PHASE::Reinforcement:
		setViewHeader(
			"=============================\n" +
			name + ": " + phaseString + "\n" +
			"=============================\n"
		);
		setViewInformation("");
		break;
	case PHASE::Drawing:
		setViewHeader("");
		setViewInformation(
			name + " drew a card: \n" +
			p->getHand().getCards().at(p->getHand().size() - 1)->printType() + "\n"
		);
		break;
	case PHASE::ShowHand:
		setViewHeader("");
		break;
	case PHASE::Exchanging:
		setViewHeader("");
		setViewInformation(
			"Reinforcements received from card exchange: " + to_string(p->getExchangeResult()) + "\n" +
			"Total reinforcements received this turn: " + to_string(p->getReinforcements()) + "\n"
		);
		break;
	case PHASE::Reinforcing:
		setViewHeader("");
		setViewInformation("");
		break;
	case PHASE::Attacking:
		setViewInformation("");
		break;
	case PHASE::BattleResults:
		setViewInformation("Here are the battle results\n");
		break;
	case PHASE::TurnOver:
		setViewHeader(
			"*********************************\n\t" +
			name + "'s Turn is over\n" +
			"*********************************\n"
		);
		break;
	case PHASE::TurnStart:
		system("cls");
		setViewHeader(
			"*********************************\n\t" +
			name + "'s Turn\n" + 
			"*********************************\n"
		);
	}
	displayView();
}

vector<shared_ptr<Player>>& Game::getPlayers()
{
	return m_players;
}

shared_ptr<Player>& Game::getActivePlayer()
{
	return m_players[m_activePlayer];
}

vector<shared_ptr<Country>> Game::getCountries()
{
	return m_map.getCountries();
}

void Game::setViewHeader(const string & str)
{
	m_viewHeader = str;
}

void Game::setViewInformation(const string & str)
{
	m_viewInformation = str;
}

string Game::getViewHeader() const
{
	return m_viewHeader;
}

string Game::getViewInformation() const
{
	return m_viewInformation;
}

void Game::displayView() const
{
	if (m_viewHeader != "")
		cout << m_viewHeader << endl;
	if (m_viewInformation != "")
		cout << m_viewInformation << endl;
}

// randomly distribute countries to each player
void Game::distributeCountries()
{
	vector<shared_ptr<Country>> vCountries = m_map.getCountries();
	while (!vCountries.empty()) {
		for (auto &itr : m_players) {
			// For each Player
			// Get a random number between 0 and the number of countries, to index the vector
			// Set that country's occupying player to current Player
			// Add that country to the vector of countries owned by current Player
			// Remove that Country from vector so it won't be assigned again

			int index = rand() % vCountries.size();
			vCountries[index]->setOccupyingPlayer(itr);
			//vCountries[index]->attach(m_statistics);
			itr->addCountry(vCountries[index]);
			vCountries.erase(vCountries.begin() + index);
			if (vCountries.empty())
				break;
		}
		// Repeat until all countries are assigned (when vector is empty)
	}
}

void Game::distributeInitialArmies()
{
	// Give each player an initial amount of armies to distribute on their countries based on the number of players:
	// 2 Players - 40
	// 3 Players - 35
	// 4 Players - 30
	// 5 Players - 25
	// 6 Players - 20
	// m_ARMY_DISTRIBUTION_VALUE is assumed to be 50 as per the assignment instructions
	int reinforcements = (m_ARMY_DISTRIBUTION_VALUE - (5 * m_numPlayers));

	// assign 1 army to each country owned by each player, round robin until their reinforcements runs out
	// another option would be to let the player choose where to place their initial armies (could be time consuming)
	for (auto &player : m_players) {
		player->addReinforcements(reinforcements);
		while (player->getReinforcements() > 0) {
			for (auto &country : player->getCountries()) {
				if (player->getReinforcements() <= 0)
					break;
				else {
					country->addToArmyValue(1);
					player->addReinforcements(-1);
				}
			}
		}
	}
}

void Game::changeActivePlayer() const
{
}

void Game::drawCard()
{
	m_players[m_activePlayer]->draw(m_deck.draw());
}

void Game::putBackCards(vector<shared_ptr<Card>> cards)
{
	if (!cards.empty())
		m_deck.put_back(cards);
}

unsigned int Game::getNumOfMaps() const
{
	return m_mapNames.size();
}

unsigned int Game::getNumOfGamesPerMap() const
{
	return m_numOfGamesPerMap;
}

unsigned int Game::getMaxTurns() const
{
	return m_maxTurns;
}

unsigned int Game::getTurnNumber() const
{
	return m_turnNumber;
}

void Game::setMaxTurns(int maxTurns)
{
	m_maxTurns = maxTurns;
}

void Game::setTurnNumber(int turnNumber)
{
	m_turnNumber = turnNumber;
}

void Game::removeConqueredPlayer()
{
	for (unsigned int i = 0; i < m_players.size(); i++) {
		if (m_players.at(i)->getCountries().empty() && !m_players.at(i)->Eliminated()) {
			m_players.at(i)->setEliminated(true);
			m_numPlayers--;
		}
	}
}

void Game::checkWinCondition()
{
	if (m_players[m_activePlayer]->getCountries().size() == m_map.getCountries().size()) {
		m_winnersName = m_players[m_activePlayer]->getStrategyName();
		declareWinner();
		m_gameOver = true;
	}
}

bool Game::gameOver() const
{
	return m_gameOver;
}

void Game::declareWinner() const
{
	cout << m_players[m_activePlayer]->name() << " won the game!" << endl;
}