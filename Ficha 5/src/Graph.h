/*
 * Graph.h
 */
#ifndef GRAPH_H_
#define GRAPH_H_

#include <vector>
#include <queue>
#include <list>
#include <limits>
#include <cmath>
#include "MutablePriorityQueue.h"

using namespace std;

template <class T> class Edge;
template <class T> class Graph;
template <class T> class Vertex;

#define INF std::numeric_limits<double>::max()

/************************* Vertex  **************************/

template <class T>
class Vertex {
	T info;                // contents
	vector<Edge<T> > adj;  // outgoing edges
	bool visited;          // auxiliary field
	double dist = 0;
	Vertex<T> *path = NULL;
	int queueIndex = 0; 		// required by MutablePriorityQueue

	bool processing = false;
	void addEdge(Vertex<T> *dest, double w);

public:
	Vertex(T in);
	bool operator<(Vertex<T> & vertex) const; // // required by MutablePriorityQueue
	T getInfo() const;
	double getDist() const;
	Vertex *getPath() const;
	friend class Graph<T>;
	friend class MutablePriorityQueue<Vertex<T>>;
};


template <class T>
Vertex<T>::Vertex(T in): info(in) {}

/*
 * Auxiliary function to add an outgoing edge to a vertex (this),
 * with a given destination vertex (d) and edge weight (w).
 */
template <class T>
void Vertex<T>::addEdge(Vertex<T> *d, double w) {
	adj.push_back(Edge<T>(d, w));
}

template <class T>
bool Vertex<T>::operator<(Vertex<T> & vertex) const {
	return this->dist < vertex.dist;
}

template <class T>
T Vertex<T>::getInfo() const {
	return this->info;
}

template <class T>
double Vertex<T>::getDist() const {
	return this->dist;
}

template <class T>
Vertex<T> *Vertex<T>::getPath() const {
	return this->path;
}

/********************** Edge  ****************************/

template <class T>
class Edge {
	Vertex<T> * dest;      // destination vertex
	double weight;         // edge weight
public:
	Edge(Vertex<T> *d, double w);
	friend class Graph<T>;
	friend class Vertex<T>;
};

template <class T>
Edge<T>::Edge(Vertex<T> *d, double w): dest(d), weight(w) {}


/*************************** Graph  **************************/

template <class T>
class Graph {
	vector<Vertex<T> *> vertexSet;    // vertex set
	vector<vector<Vertex<T>*>> pathMatrix;
	vector<vector<double>> distMatrix;

public:
	Vertex<T> *findVertex(const T &in) const;
	bool addVertex(const T &in);
	bool addEdge(const T &sourc, const T &dest, double w);
	int getNumVertex() const;
	vector<Vertex<T> *> getVertexSet() const;

	// Fp05 - single source
	void dijkstraShortestPath(const T &s);
	void dijkstraShortestPathOld(const T &s);
	void unweightedShortestPath(const T &s);
	void bellmanFordShortestPath(const T &s);
	vector<T> getPath(const T &origin, const T &dest) const;

	// Fp05 - all pairs
	void floydWarshallShortestPath();
	vector<T> getfloydWarshallPath(const T &origin, const T &dest) const;
};

template <class T>
int Graph<T>::getNumVertex() const {
	return vertexSet.size();
}

template <class T>
vector<Vertex<T> *> Graph<T>::getVertexSet() const {
	return vertexSet;
}

/*
 * Auxiliary function to find a vertex with a given content.
 */
template <class T>
Vertex<T> * Graph<T>::findVertex(const T &in) const {
	for (auto v : vertexSet)
		if (v->info == in)
			return v;
	return NULL;
}

/*
 *  Adds a vertex with a given content or info (in) to a graph (this).
 *  Returns true if successful, and false if a vertex with that content already exists.
 */
template <class T>
bool Graph<T>::addVertex(const T &in) {
	if ( findVertex(in) != NULL)
		return false;
	vertexSet.push_back(new Vertex<T>(in));
	return true;
}

/*
 * Adds an edge to a graph (this), given the contents of the source and
 * destination vertices and the edge weight (w).
 * Returns true if successful, and false if the source or destination vertex does not exist.
 */
template <class T>
bool Graph<T>::addEdge(const T &sourc, const T &dest, double w) {
	auto v1 = findVertex(sourc);
	auto v2 = findVertex(dest);
	if (v1 == NULL || v2 == NULL)
		return false;
	v1->addEdge(v2,w);
	return true;
}


/**************** Single Source Shortest Path algorithms ************/

template<class T>
void Graph<T>::dijkstraShortestPath(const T &origin)
{
	for (auto v : vertexSet)
	{
		v->dist = numeric_limits<double>::max();
		v->path = NULL;
	}

	Vertex<T> *s = findVertex(origin);
	s->dist = 0;

	MutablePriorityQueue<Vertex<T>> q;
	q.insert(s);

	Vertex<T> *v, *w;
	while(!q.empty())
	{
		v = q.extractMin();
		for (auto e : v->adj)
		{
			w = e.dest;
			bool add = (w->dist == numeric_limits<double>::max());
			if(w->dist > v->dist + e.weight)
			{
				w->dist = v->dist + e.weight;
				w->path = v;

				if(add)
				{
					q.insert(w);
				}
				else
				{
					q.decreaseKey(w);
				}
			}
		}
	}
}

template<class T>
vector<T> Graph<T>::getPath(const T &origin, const T &dest) const
{
	vector<T> res;

	Vertex<T> *current = findVertex(dest);
	res.push_back(current->info);
	while(current->path != NULL)
	{
		res.push_back(current->path->info);
		current = current->path;
	}

	reverse(res.begin(),res.end());
	return res;
}

template<class T>
void Graph<T>::unweightedShortestPath(const T &orig)
{
	for (auto v : vertexSet)
	{
		v->dist = numeric_limits<double>::max();
		v->path = NULL;
	}

	Vertex<T> *s = findVertex(orig);
	s->dist = 0;

	queue<Vertex<T>*> q;
	q.push(s);

	Vertex<T> *v, *w;
	while(!q.empty())
	{
		v = q.front();
		q.pop();
		for (auto e : v->adj)
		{
			w = e.dest;
			if(w->dist == numeric_limits<double>::max())
			{
				q.push(w);
				w->dist = v->dist + 1;
				w->path = v;
			}
		}
	}

//	for (auto v : vertexSet)
//	{
//		if(v->path != NULL)
//			cout << v->path->info << endl;
//	}
}

template<class T>
void Graph<T>::bellmanFordShortestPath(const T &orig)
{
	for (auto v : vertexSet)
	{
		v->dist = numeric_limits<double>::max();
		v->path = NULL;
	}

	findVertex(orig)->dist = 0;

	size_t itrs = this->getNumVertex();
	Vertex<T> *w;
	bool updated = false;
	for (size_t i = 1; i < itrs - 1; ++i)
	{
		updated = false;

		for (auto v : vertexSet)
		{
			if(v->dist == numeric_limits<double>::max())
				continue;

			for (auto e : v->adj)
			{
				w = e.dest;
				//cout << w->info << " : " << v->info << endl;
				if(w->dist > v->dist + e.weight)
				{
					w->dist = v->dist + e.weight;
					w->path = v;
					updated = true;
				}
			}
		}

		if(!updated)
			break;
	}

	for (auto v : vertexSet)
	{
		for (auto e : v->adj)
		{
			w = e.dest;
			if(v->dist + e.weight < w->dist)
			{
				cout << "Não há solução" << endl;
				return;
			}
		}
	}
}


/**************** All Pairs Shortest Path  ***************/

template<class T>
void Graph<T>::floydWarshallShortestPath()
{
	int size = getNumVertex();

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			if(i == j)
				distMatrix.at(i).at(j) = 0;
			else
				distMatrix.at(i).at(j) = INF;

			pathMatrix.at(i).at(j) = NULL;
		}
	}


	for(int k = 1; k < getNumVertex(); k++)
	{
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				//D[i,j](k)= min( D[i,j](k-1), D[i,k](k-1) + D[k,j](k-1))
				double old = distMatrix.at(i).at(j);
				double newD = distMatrix.at(i).at(k) + distMatrix.at(k).at(j);
				if(newD < old)
				{
					distMatrix.at(i).at(j) = newD;
					pathMatrix.at(i).at(j) = vertexSet.at(k);
				}
			}
		}
	}
}

template<class T>
vector<T> Graph<T>::getfloydWarshallPath(const T &orig, const T &dest) const{
	vector<T> res;
//
//	Vertex<T> *vO;
	int idOrigin = findv
//
	return res;
}


#endif /* GRAPH_H_ */
