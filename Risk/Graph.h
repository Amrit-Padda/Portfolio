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


// A class to represent a graph data structure
// the Graph class uses an adjacency list to store the structure (a vector<vector<int>>)
//		the first dimension of the adjacency list is a vector of all the nodes. Let i denote this node
//		the second dimension is a vector of all nodes that are adjacent to the ith node
// the graph can be traversed using a breadth first search
class Graph {
private:
	int					m_numberOfVertices;
	vector<vector<int>>	m_adj;

public:
	Graph(int);
	Graph(int, vector<vector<int>>);

	void	addEdge(int, int);
	bool	isAdjacentTo(int, int);
	bool	BFS(int);
	int		getNumberOfVertices();
};