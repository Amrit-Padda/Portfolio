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
#include "Observer.h"

using std::list;

class Subject {
public:
	virtual void attach(Observer *o);
	virtual void detach(Observer *o);
	virtual void notify();
	Subject();
	~Subject();

private:
	list <Observer*> *m_pObservers;
};
