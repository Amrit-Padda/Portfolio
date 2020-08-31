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

#include "Dice.h"
#include <functional>

Dice::Dice() :
	m_numRolls(0), m_counts({0, 0, 0, 0, 0, 0}), m_percentages({0.0, 0.0, 0.0, 0.0, 0.0, 0.0})
{
}

// roll a number of dice
// update the occurences of each dice value and total rolls
// store the results in a sorted vector and return it
vector<int> Dice::roll(int number)  {
	m_result.clear();
	for (int i = 0 ; i < number; i++) {	
		int randnum = (rand() % 6) + 1;
		m_numRolls++; //adding +1 to the total number of rolls
		m_counts[randnum - 1]++ ; //adding value to counts vector, note that we subtract 1 since array is indexed at 0
		m_result.push_back(randnum) ;	//pushing the random number to the result vector
	}

	sort(m_result.begin(), m_result.end(), greater<int>());
	return m_result;
}

void Dice::printResult() {
	cout << "Results: " ;
	for (unsigned int i = 0 ; i < m_result.size() ; i++) {
		cout << m_result[i] << "   " ;
	}
	cout << endl;
}

// use the occurences of each dice value and the total rolls to calculate the percentages of each dice value
vector<double> Dice::calPercentages() {
	if(m_numRolls != 0)
		for (int i = 0; i < 6 ; i++)
			m_percentages[i] = (double)m_counts[i] / m_numRolls;

	return m_percentages;
}