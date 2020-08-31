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

class Observer {
public:
	virtual void update() = 0;
	virtual ~Observer() = 0;
private:

protected:
	Observer();

};