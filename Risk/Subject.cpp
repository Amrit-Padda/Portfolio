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

#include "Subject.h"

void Subject::attach(Observer * o)
{
	m_pObservers->push_back(o);
}

void Subject::detach(Observer * o)
{
	m_pObservers->remove(o);
}

void Subject::notify(){

	list<Observer*>::iterator itr = m_pObservers->begin();

	for (; itr != m_pObservers->end(); itr++) {
		(*itr)->update();
	}
}

Subject::Subject()
{
	m_pObservers = new list<Observer*>;
}

Subject::~Subject()
{
	delete m_pObservers;
}
