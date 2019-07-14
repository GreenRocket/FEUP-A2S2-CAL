/*
 * labirinth.h
 *
 */

#ifndef LABIRINTH_H_
#define LABIRINTH_H_


class Labirinth {
	int labirinth[10][10];
	bool visited[10][10];
	void initializeVisited();
public:
	Labirinth(int values[10][10]);
	void printLabirinth();
	void printLabirinthVisits();
	bool findGoal(int x, int y);
	bool recursiveSearch (int x, int y);
};

#endif /* LABIRINTH_H_ */
