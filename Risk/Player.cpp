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

#include "Player.h"
#include "Country.h"
#include "HumanStrategy.h"

using namespace std;
using std::string;

unsigned int Player::m_staticId = 0;

Player::Player() :
	m_id(m_staticId), m_name(""), m_reinforcements(0), m_exchangeResult(0), m_activity(false), m_eliminated(false)
{
	m_staticId++;
}

Player::Player(string &name) :
	m_id(m_staticId), m_name(name), m_reinforcements(0), m_exchangeResult(0), m_activity(false), m_eliminated(false)
{
	++m_staticId;
}

Player::Player(Player *player) :
	m_id(player->m_id),
	m_name(player->m_name),
	m_reinforcements(player->m_reinforcements),
	m_countries(player->m_countries),
	m_dice(player->m_dice),
	m_hand(player->m_hand),
	m_exchangeResult(player->m_exchangeResult),
	m_strategy(player->m_strategy),
	m_activity(false),
	m_eliminated(player->m_eliminated)
{
}

shared_ptr<Country> Player::findCountry(vector<shared_ptr<Country>> &vpc, int id)
{
	for (auto &iter : vpc) {
		if (id == iter->getCountryId())
			return iter;
	}
	return nullptr;
}

int Player::chooseDefensiveDice(shared_ptr<Country> &country)
{
	return m_strategy->chooseDefensiveDice(country);
}

vector<int> Player::roll(int n)
{
	return m_dice.roll(n);
}

vector<double> Player::getPercentages()
{
	return m_dice.calPercentages();
}

int Player::exchange()
{
	return m_hand.exchange();
}

vector<shared_ptr<Card>> Player::returnCardsToDeck()
{
	return m_hand.mostRecentlyExchanged();
}

void Player::reinforce(int continentBonus)
{
	m_strategy->reinforce(continentBonus);
}

void Player::attack()
{
	m_strategy->attack();
}

void Player::fortify()
{
	m_strategy->fortify();
}

void Player::draw(shared_ptr<Card> &c)
{
	// draw card, add to hand
	// Game object will draw() from its member Deck and pass card to active player
	m_hand.draw(c);

	m_currentPhase = PHASE::Drawing;
	notify();
}

void Player::showHand()
{
	m_currentPhase = PHASE::ShowHand;
	notify();
	m_hand.showHand();
}

string Player::name() const
{
	return m_name;
}

int Player::id() const
{
	return m_id;
}

vector<shared_ptr<Country>>& Player::getCountries()
{
	return m_countries;
}

int Player::getReinforcements() const
{
	return m_reinforcements;
}

int Player::getHandSize() const
{
	return m_hand.size();
}

string Player::getStrategyName() const
{
	return m_strategyName;
}

bool Player::Eliminated() const
{
	return m_eliminated;
}

void Player::setStrategy(shared_ptr<Strategy>& strategy)
{
	m_strategy = strategy;
}

void Player::setCountries(vector<shared_ptr<Country>> &)
{
}

void Player::setReinforcements(int n)
{
	m_reinforcements = n;
}

void Player::addCountry(shared_ptr<Country> &c)
{
	m_countries.push_back(c);
}

void Player::addReinforcements(int r)
{
	m_reinforcements += r;
}

void Player::setActive(bool active)
{
	m_activity = active;

	if (m_activity)
		m_currentPhase = PHASE::TurnStart;
	else
		m_currentPhase = PHASE::TurnOver;
	notify();
}

void Player::setPhase(PHASE phase)
{
	m_currentPhase = phase;
}

void Player::setEliminated(bool eliminated)
{
	m_eliminated = eliminated;
}

void Player::setStrategyName(string strategyName)
{
	m_strategyName = strategyName;
}

void Player::clear()
{
	m_countries.clear();
	m_hand.clear();
	m_eliminated = false;
}

void Player::removeCountry(int id)
{
	// erase / remove idiom
	// we want to erase the country if the id matches
	// use a lambda to capture the iterator of remove_if() as a shared_ptr<Country>
	// use comparison of m_globalID of the captured object with id as a predicate for remove_if() (true / false)
	// if there's a match, remove_if() moves the object to the end of the container, where erase() erases it
	m_countries.erase(
		remove_if(
			m_countries.begin(),
			m_countries.end(),
			[&](const shared_ptr<Country> &pc) { return pc->getCountryId() == id; }
		),
		m_countries.end()
	);
}

string Player::getPhaseAsString(PHASE p)
{
	switch (p) {
	case PHASE::Attack:
		return "Attack";
	case PHASE::Attacking:
		return "Attacking";
	case PHASE::BattleResults:
		return "Battle Results";
	case PHASE::ShowHand:
		return "Show Hand";
	case PHASE::TurnOver:
		return "Turn Over";
	case PHASE::TurnStart:
		return "Turn Start";
	case PHASE::Fortification:
		return "Fortification";
	case PHASE::Reinforcement:
		return "Reinforcement";
	case PHASE::Exchanging:
		return "Exchanging";
	case PHASE::Drawing:
		return "Drawing";
	default: return "Attacking";
	}
}

PHASE Player::getCurrentPhase() const
{
	return m_currentPhase;
}

bool Player::getActive()
{
	return m_activity;
}

unsigned int Player::getExchangeResult() const
{
	return m_exchangeResult;
}

Hand Player::getHand()
{
	return m_hand;
}

bool Player::moveArmies(shared_ptr<Country> src, shared_ptr<Country> target, int toBeMoved)
{
	int srcArmy = src->getArmyValue();
	int trgtArmy = target->getArmyValue();
	if (srcArmy - toBeMoved < 1)
		return false;

	src->setArmyValue(srcArmy - toBeMoved);
	target->setArmyValue(trgtArmy + toBeMoved);

	return true;
}

