/*
 * labirinth.cpp
 */

#include "Labirinth.h"

#include <iostream>
using namespace std;



Labirinth::Labirinth(int values[10][10])
{
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
			labirinth[i][j] = values[i][j];
}


void Labirinth::initializeVisited()
{
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
			visited[i][j] = false;
}




void Labirinth::printLabirinth()
{
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
			cout << labirinth[i][j] << " ";

		cout << endl;
	}
}

void Labirinth::printLabirinthVisits()
{
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
			cout << (visited[i][j] ? 'o' : '#') << " ";

		cout << endl;
	}
}


bool Labirinth::findGoal(int x, int y)
{
	initializeVisited();

	if (recursiveSearch(x, y))
	{
		return true;
	}
	else
	{
		cout << "IMPOSSIBLE" << endl;
		return false;
	}
}

bool Labirinth::recursiveSearch(int x, int y)
{
	// Saida
	if (labirinth[x][y] == 2)
	{
		cout << "EXIT: " << x << ":" << y << endl;
		return true;
	}

	// Parede ou já cá tivemos
	if (labirinth[x][y] == 0 || visited[x][y])
		return false;

	// Espaço livre nunca vizitado. Marcar como visitado
	visited[x][y] = true;

//	int c = labirinth[x][y];
//	labirinth[x][y] = 9;
//	printLabirinth();
//	labirinth[x][y] = c;
//	cout << endl << endl;
//	printLabirinthVisits();
//	cout << endl << endl;

	// Verificar vizinhança
	return recursiveSearch(x-1, y) || recursiveSearch(x+1, y) || recursiveSearch(x, y-1) || recursiveSearch(x, y+1);
}


