/*
 * Graph.h
 */
#ifndef GRAPH_H_
#define GRAPH_H_

#include <vector>
#include <queue>
using namespace std;

template <class T> class Edge;
template <class T> class Graph;
template <class T> class Vertex;


/****************** Provided structures  ********************/

template <class T>
class Vertex {
	T info;                // contents
	vector<Edge<T> > adj;  // list of outgoing edges
	bool visited;          // auxiliary field used by dfs and bfs
	int indegree;          // auxiliary field used by topsort
	bool processing;       // auxiliary field used by isDAG

	void addEdge(Vertex<T> *dest, double w);
	bool removeEdgeTo(Vertex<T> *d);
public:
	Vertex(T in);
	friend class Graph<T>;
};

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
class Graph {
	vector<Vertex<T> *> vertexSet;    // vertex set

	void dfsVisit(Vertex<T> *v,  vector<T> & res) const;
	Vertex<T> *findVertex(const T &in) const;
	bool dfsIsDAG(Vertex<T> *v) const;
public:
	size_t getNumVertex() const;
	bool addVertex(const T &in);
	bool removeVertex(const T &in);
	bool addEdge(const T &sourc, const T &dest, double w);
	bool removeEdge(const T &sourc, const T &dest);
	vector<T> dfs() const;
	vector<T> bfs(const T &source) const;
	vector<T> topsort() const;
	int maxNewChildren(const T &source, T &inf) const;
	bool isDAG() const;
};

/****************** Provided constructors and functions ********************/

template <class T>
Vertex<T>::Vertex(T in): info(in) {}

template <class T>
Edge<T>::Edge(Vertex<T> *d, double w): dest(d), weight(w) {}

template <class T>
size_t Graph<T>::getNumVertex() const {
	return vertexSet.size();
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

/****************** 1a) addVertex ********************/

/*
 *  Adds a vertex with a given content/info (in) to a graph (this).
 *  Returns true if successful, and false if a vertex with that content already exists.
 */
template <class T>
bool Graph<T>::addVertex(const T &in)
{
	// use the findVertex function to check if a vertex already exists
	if(findVertex(in))
		return false;

	// vertexSet é um vetor de POINTERS para vertex
	// logo é necessário criar o vertex na heap
	// caso contrário ele é apagado ao sair da scope desta função

	// Vertex<T> v(in); 			 	// Criado na stack -> ERRADO
	// vertexSet.push_back(&v);

	Vertex<T> *v = new Vertex<T>(in);	// Criado na heap -> CORRETO
	vertexSet.push_back(v);
	return true;
}

/****************** 1b) addEdge ********************/

/*
 * Adds an edge to a graph (this), given the contents of the source (sourc) and
 * destination (dest) vertices and the edge weight (w).
 * Returns true if successful, and false if the source or destination vertex does not exist.
 */
template <class T>
bool Graph<T>::addEdge(const T &sourc, const T &dest, double w)
{
	// use findVertex to obtain the actual vertices (if they exist)
	Vertex<T> *vs, *vd;
	if( !(vs = findVertex(sourc)) || !(vd = findVertex(dest)))
		return false;

	// use the next function to actually add the edge
	vs->addEdge(vd, w);
	//vd->addEdge(vs, w);
	return true;
}

/*
 * Auxiliary function to add an outgoing edge to a vertex (this),
 * with a given destination vertex (d) and edge weight (w).
 */
template <class T>
void Vertex<T>::addEdge(Vertex<T> *d, double w) {
	Edge<T> edge(d, w);
	this->adj.push_back(edge);
}

/****************** 1c) removeEdge ********************/

/*
 * Removes an edge from a graph (this).
 * The edge is identified by the source (sourc) and destination (dest) contents.
 * Returns true if successful, and false if such edge does not exist.
 */
template <class T>
bool Graph<T>::removeEdge(const T &sourc, const T &dest)
{
	// HINT: Use "findVertex" to obtain the actual vertices.
	Vertex<T> *v, *d;
	if( !(v = findVertex(sourc)) || !(d = findVertex(dest)) )
		return false;

	// HINT: Use the next function to actually remove the edge.
	return v->removeEdgeTo(d);
}

/*
 * Auxiliary function to remove an outgoing edge (with a given destination (d))
 * from a vertex (this).
 * Returns true if successful, and false if such edge does not exist.
 */
template <class T>
bool Vertex<T>::removeEdgeTo(Vertex<T> *d)
{
	if(this == d)
		return false;

	// HINT: use an iterator to scan the "adj" vector and then erase the edge.
	for (typename vector<Edge<T>>::iterator i = this->adj.begin(); i != this->adj.end(); i++)
	{
		// Verificar se é o vertice destino.
		if( (*i).dest == d )
		{
			// Remover aresta.
			this->adj.erase(i);
			return true;
		}
	}

	// Nenhuma aresta tem o vertice destino, logo não existe aresta para este vertice.
	return false;
}

/****************** 1d) removeVertex ********************/

/*
 *  Removes a vertex with a given content (in) from a graph (this), and
 *  all outgoing and incoming edges.
 *  Returns true if successful, and false if such vertex does not exist.
 */
template <class T>
bool Graph<T>::removeVertex(const T &in)
{
	//Vertex<T> *vertex;

	// HINT: use an iterator to scan the "vertexSet" vector and then erase the vertex.
	for (typename vector<Vertex<T>*>::iterator i = this->vertexSet.begin(); i != this->vertexSet.end(); i++)
	{
		//vertex = *i;

		// Verificar se vertice é o que procuramos.
		if( !((*i)->info == in) )
			continue;

		// Remover arestas de outros vertices cujo destino é o vertice que queremos remover.
		for (auto other : vertexSet)
			other->removeEdgeTo((*i));

		// Remover da lista de vertices.
		this->vertexSet.erase(i);

		// Libertar memoria alocada pelo vertice na heap.
		delete (*i);

		return true;
	}
	return false;
}

/****************** 2a) dfs ********************/

/*
 * Performs a depth-first search (dfs) in a graph (this).
 * Returns a vector with the contents of the vertices by dfs order.
 * Follows the algorithm described in theoretical classes.
 */
template <class T>
vector<T> Graph<T>::dfs() const
{
	// G = Graph
	// E = edges
	// V = vertices
	// Adj(v) = {w | (v, w) in E, v in V}

	vector<T> res;

	// Limpar flag de potencial pesquisa anterior.
	for (auto v : vertexSet)										// for each v in V
		v->visited = false;											//		visited(v) <-- false
																	//
	// A cada vertice não visitado, visitar os vertices ligados.	//
	for (auto v : vertexSet)										// for each v in V
		if(!v->visited)												//		if not visited(v)
			this->dfsVisit(v, res);									//			DFS-VISIT(G,v)

	return res;
}

/*
 * Auxiliary function that visits a vertex (v) and its adjacent not yet visited, recursively.
 * Updates a parameter with the list of visited node contents.
 */
template <class T>
void Graph<T>::dfsVisit(Vertex<T> *v, vector<T> & res) const
{
	Vertex<T> *w;

	// Marcar vertice como visitado
	v->visited = true;												// visited(v) <-- true
	// Adicionar conteudo do vertice à lista resultado				//
	res.push_back(v->info);											// process(v)
																	//
	// Percorrer recursivamente os ramos ligados a este vertice.	//
	for (auto edge : v->adj){										// for each edge in Adj(v)
		w = edge.dest;												// 		let w be the edge's destination
		if(!w->visited)												// 		if not visited(w)
			this->dfsVisit(w, res);									// 			DFS-VISIT(G, w)
	}
}

/****************** 2b) bfs ********************/

/*
 * Performs a breadth-first search (bfs) in a graph (this), starting
 * from the vertex with the given source contents (source).
 * Returns a vector with the contents of the vertices by bfs order.
 * Follows the algorithm described in theoretical classes.
 */
template <class T>
vector<T> Graph<T>::bfs(const T & source) const
{
	vector<T> res;
	Vertex<T> *vs, *vTemp, *wTemp;
	if(!(vs = findVertex(source)))
		return res;

	// Limpar flag de potencial pesquisa anterior.
	for (auto v : vertexSet)										// for each v in V
		v->visited = false;											//		do discovered(v) <-- false
	queue<Vertex<T> *> notProcessed;								// notProcessed <-- EMPTY
																	//
	// Adicionar vertice inicial à pilha e marcar como visitado		//
	notProcessed.push(vs);											// ENQUEUE(notProcessed,vs)
	vs->visited = true;												// discovered(vs) <-- true
																	//
	// Enquanto houver vertices for visitar							//
	while(notProcessed.size() > 0){									// while notProcessed is not empty
		// Remover vertice do topo da queue							//
		vTemp = notProcessed.front();								//		temp <-- DEQUEUE(notProcessed)
		notProcessed.pop();											//
		// Adicionar conteudo do vertice à lista resultado			//
		res.push_back(vTemp->info);									// 		pre-process(v)
		// Adicionar à pilha os vertices ligados a este vertice		//
		for (auto edge : vTemp->adj){								// 		for each edge in Adj(vTemp)
			wTemp = edge.dest;										// 			let wTemp be the edge's destination
			if(!wTemp->visited){									// 			if not discovered(wTemp) then
				notProcessed.push(wTemp);							//				ENQUEUE(notProcessed,wTemp)
				wTemp->visited = true;								//				discovered(wTemp) <-- true
			}
		}
	}
	return res;
}

/****************** 2c) toposort ********************/

/*
 * Performs a topological sorting of the vertices of a graph (this).
 * Returns a vector with the contents of the vertices by topological order.
 * If the graph has cycles, returns an empty vector.
 * Follows the algorithm described in theoretical classes.
 */
template<class T>
vector<T> Graph<T>::topsort() const									// TOP-SORT( in G=(V,E), out T):
{
	for (auto v : vertexSet)										// for each v in V
		v->indegree = 0;											//		do indegree(v) <-- 0
																	//
	for (auto v : vertexSet)										// for each v in V
		for (auto edge : v->adj)									// 		for each edge in Adj(v)
			edge.dest->indegree++;									// 			do indegree(w) <-- indegree(w) + 1 where w is the edge's destination vertex
																	//
	queue<Vertex<T>*> candidates;									// candidates <-- { }
	for (auto v : vertexSet)										// for each v in V
		if(v->indegree == 0)										//		if indegree(v) = 0 then
			candidates.push(v);										//			 candidates <-- candidates U {v}
																	//
	vector<T> res;													// res <-- [ ]
	Vertex<T> *vTemp, *wTemp;										//
	while(candidates.size() > 0){									// while candidates is not empty
		vTemp = candidates.front();									//		vTemp <-- remove-one(candidates)
		candidates.pop();											//
		res.push_back(vTemp->info);									// 		res <-- res concatenado-com [vTemp]
																	//
		for (auto edge : vTemp->adj){								// 		for each edge in Adj(vTemp)
			wTemp = edge.dest;										// 			let wTemp be the edge's destination
			wTemp->indegree--;										//			indegree(wTemp) <-- indegree(wTemp) – 1
			if(wTemp->indegree == 0)								//			if indegree(w) = 0
				candidates.push(wTemp);								//				candidates <-- candidates U {wTemp}
		}															//
	}																//
																	//
	if(res.size() != this->getNumVertex())							// if |T| != |V| then Fail(“O grafo tem ciclos”)
		res.clear();

	return res;
}

/****************** 3a) maxNewChildren (HOME WORK)  ********************/

/*
 * Performs a breadth-first search in a graph (this), starting
 * from the vertex with the given source contents (source).
 * During the search, determines the vertex that has a maximum number
 * of new children (adjacent not previously visited), and returns the
 * contents of that vertex (inf) and the number of new children (return value).
 */
template <class T>
int Graph<T>::maxNewChildren(const T & source, T &inf) const
{
	int max = -1, curr = 0;

	Vertex<T> *vs, *vTemp, *wTemp;
	if(!(vs = findVertex(source)))
		return max;

	// Limpar flag de potencial pesquisa anterior.
	for (auto v : vertexSet)										// for each v in V
		v->visited = false;											//		do discovered(v) <-- false
	queue<Vertex<T> *> notProcessed;								// notProcessed <-- EMPTY
																	//
	// Adicionar vertice inicial à pilha e marcar como visitado		//
	notProcessed.push(vs);											// ENQUEUE(notProcessed,vs)
	vs->visited = true;												// discovered(vs) <-- true
																	//
	// Enquanto houver vertices for visitar							//
	while(notProcessed.size() > 0){									// while notProcessed is not empty
		// Remover vertice do topo da queue							//
		vTemp = notProcessed.front();								//		temp <-- DEQUEUE(notProcessed)
		notProcessed.pop();											//
		curr = 0;
		// Adicionar à pilha os vertices ligados a este vertice		//
		for (auto edge : vTemp->adj){								// 		for each edge in Adj(vTemp)
			wTemp = edge.dest;										// 			let wTemp be the edge's destination
			if(!wTemp->visited){									// 			if not discovered(wTemp) then
				notProcessed.push(wTemp);							//				ENQUEUE(notProcessed,wTemp)
				wTemp->visited = true;								//				discovered(wTemp) <-- true
				curr++;
			}
		}
		if(curr > max)
		{
			max = curr;
			inf = vTemp->info;
		}

	}
	return max;
}

/****************** 3b) isDAG   (HOME WORK)  ********************/

/*
 * Performs a depth-first search in a graph (this), to determine if the graph
 * is acyclic (acyclic directed graph or DAG).
 * During the search, a cycle is found if an edge connects to a vertex
 * that is being processed in the the stack of recursive calls (see theoretical classes).
 * Returns true if the graph is acyclic, and false otherwise.
 */
template <class T>
bool Graph<T>::isDAG() const
{
	//bool res = true;

	for (auto v : vertexSet)
	{
		v->visited = false;
		v->processing = false;
	}
	for (auto v : vertexSet)
	{
		if(!v->visited)
		{
			//v->processing = true;
			if(!this->dfsIsDAG(v))
				return false;
			//v->processing = false;
		}
	}
	return true;
}

/**
 * Auxiliary function that visits a vertex (v) and its adjacent not yet visited, recursively.
 * Returns false (not acyclic) if an edge to a vertex in the stack is found.
 */
template <class T>
bool Graph<T>::dfsIsDAG(Vertex<T> *v) const
{
	Vertex<T> *w;

	v->processing = true;
	v->visited = true;
	for (auto edge : v->adj)
	{
		w = edge.dest;

		if(w->processing)
			return false;

		if(!w->visited)
			if(!this->dfsIsDAG(w))
				return false;
	}
	v->processing = false;
	return true;
}

#endif /* GRAPH_H_ */
