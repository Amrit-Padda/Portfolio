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

using namespace std;

// A class to represent a set of dice owned by a player in the game Risk
// each player has their own set of dice
// Dice has methods to roll a user specified number of dice and return the results
// a running count of the dice values rolled so far, and the percentages of each dice value are maintained for each Dice object
class Dice {
	int				m_numRolls;	//storing number of rolls
	vector<int>		m_result;	//vector to store the rolls
	vector<int>		m_counts; // vector to store the number of die rolled
	vector<double>	m_percentages; //vector to store percentages

public:
	Dice(); //default constructor

	vector<int>		roll(int); //roll function that takes as parameter the number of die required
	void			printResult(); //printing an unsorted result vector
	vector<double>	calPercentages(); //calculating the percentages
};