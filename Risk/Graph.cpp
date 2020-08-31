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

#include "Graph.h"

Graph::Graph(int n)
{
	m_numberOfVertices = n;
	m_adj = vector<vector<int>>(n);
}

Graph::Graph(int n, vector<vector<int>> list)
{
	m_numberOfVertices = n;
	m_adj = vector<vector<int>>(n);
	
	for (unsigned int i = 0; i < list.size(); i++) {
		for (unsigned int j = 0; j < list.at(i).size(); j++) {
			m_adj[i].push_back(list.at(i).at(j));
		}
	}
}

void Graph::addEdge(int u, int v)
{
	m_adj[u].push_back(v);
}

bool Graph::isAdjacentTo(int u, int v)
{
	for (unsigned int i = 0; i < m_adj[u].size(); i++) {
		if (m_adj[u][i] == v)
			return true;
	}
	return false;
}


/*
	BFS ALGORITHM
		Copied from https://www.geeksforgeeks.org/breadth-first-search-or-bfs-for-a-graph/
		for the algorithm

		Returns true if the traversal of all nodes was completed, otherwise false.
*/
bool Graph::BFS(int s)
{
	int V = m_numberOfVertices;

	// Mark all the vertices as not visited
	bool *visited = new bool[V];
	for (int i = 0; i < V; i++)
		visited[i] = false;

	// Create a queue for BFS
	list<int> queue;

	// Mark the current node as visited and enqueue it
	visited[s] = true;
	queue.push_back(s);

	while (!queue.empty())
	{
		s = queue.front();
		// Dequeue a vertex from queue and print it
		queue.pop_front();

		// Get all adjacent vertices of the dequeued
		// vertex s. If a adjacent has not been visited, 
		// then mark it visited and enqueue it
		for (vector<int>::iterator &i = m_adj.at(s).begin(); i != m_adj.at(s).end(); i++)
		{
			if (!visited[*i])
			{
				visited[*i] = true;
				queue.push_back(*i);
			}
		}
	}

	//If at least one of the country is not connected, then the graph is not valid
	for (int i = 0; i < V; i++) {
		if (!visited[i]) {
			return false;
		}
	}
	delete [] visited;
	visited = NULL;
	return true;
}

int Graph::getNumberOfVertices()
{
	return m_numberOfVertices;
}
