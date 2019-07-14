#include <algorithm> 	//shuffle
#include <cmath>
#include <fstream>
#include <iostream>
#include <random> 		//mt19937
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <chrono>

#include "Graph.h"
#include "graphviewer.h"
#include "ParsingHelper.h"

//ln -s /mnt/c/Program\ Files\ \(x86\)/Java/jre1.8.0_151/bin/java.exe /bin/java

const int CENTRO_APOIO = 449615230;

const string NODE_DEFAULT_COLOR = LIGHT_GRAY;
const int NODE_DEFAULT_SIZE = 20;

const string PACKAGE_ORIG_COLOR = GREEN;
const string PACKAGE_DEST_COLOR = RED;
const int PACKAGE_DEFAULT_SIZE = 60;

const string PATH_HIGHLIGHT_COLOR = CYAN;
const string PATH_DEFAULT_COLOR = BLUE;
const int PATH_HIGHLIGHT_SIZE = 40;
const int PATH_DEFAULT_SIZE = 10;


struct Node
{
	int id;
	double x;
	double y;

	Node() {}
	Node(int id, float x, float y) : id(id), x(x), y(y) {}
	bool operator==(const Node& other) { return (this->id == other.id); }
};

struct Package
{
	int id;
	int edgedID;
	double distance;
	Vertex<Node>* orig;
	Vertex<Node>* dest;

	Package() {}
	Package(int id, int edgedID, double distance, Vertex<Node>* orig, Vertex<Node>* dest) : id(id), edgedID(edgedID), distance(distance), orig(orig), dest(dest) {}
};

struct Route
{
	int ID;
	double totalDistance;
	vector<int> nodeIDs;
	vector<int> edgeIDs;

	Route() {}
	Route(int ID, double totalDistance, vector<int> nodeIDs, vector<int> edgeIDs) : ID(ID), totalDistance(totalDistance), nodeIDs(nodeIDs), edgeIDs(edgeIDs) {}
};

void drawRoute(GraphViewer *gv, const Route& r)
{
	for(auto& id : r.edgeIDs)
	{
		gv->setEdgeColor(id, BLUE);
		gv->setEdgeThickness(id, 7);
		gv->setEdgeDashed(id, false);
	}

	gv->rearrange();
}

void drawPackage(GraphViewer *gv, const Package& p)
{
	int origID = p.orig->getInfo().id;
	int destID = p.dest->getInfo().id;
	
	gv->setVertexColor(origID, PACKAGE_ORIG_COLOR);
	gv->setVertexColor(destID, PACKAGE_DEST_COLOR);
	gv->setVertexSize(origID, PACKAGE_DEFAULT_SIZE);
	gv->setVertexSize(destID, PACKAGE_DEFAULT_SIZE);

	gv->addEdge(p.edgedID, origID, destID, EdgeType::DIRECTED);
	gv->setEdgeColor(p.edgedID, CYAN);
	gv->setEdgeThickness(p.edgedID, 5);
	gv->setEdgeDashed(p.edgedID, false);

	gv->rearrange();
}

void clearPackage(GraphViewer *gv, const Package& p)
{
	int origID = p.orig->getInfo().id;
	int destID = p.dest->getInfo().id;
	
	gv->setVertexColor(origID, NODE_DEFAULT_COLOR);
	gv->setVertexColor(destID, NODE_DEFAULT_COLOR);
	gv->setVertexSize(origID, NODE_DEFAULT_SIZE);
	gv->setVertexSize(destID, NODE_DEFAULT_SIZE);

	gv->removeEdge(p.edgedID);

	gv->rearrange();
}

int nodeFileToGraph(Graph<Node>& graph, const string &filePath)
{
	ifstream inputFile;
	if (ParsingHelper::openFileRead(inputFile, filePath) == -1)
		return -1;

	// nº de nodes no ficheiro
	string temp;
	unsigned nodeCount;
	getline(inputFile, temp);
	if (!ParsingHelper::safeStoul(nodeCount, temp, 0, numeric_limits<unsigned>::max()))
		return -1;

	bool first = false;
	double x0;
	double y0;
	char c;
	while (inputFile.good())
	{
		getline(inputFile, temp);
		istringstream ss(temp);

		Node n;
		ss >> c;		// Skip '('
		ss >> n.id; 	// Get ID
		ss >> c;		// Skip ','
		ss >> n.x;		// Get X coord
		ss >> c;		// Skip ','
		ss >> n.y;		// Get Y coord

		if(!first)
		{
			x0 = n.x;
			y0 = n.y;
			first = true;
		}

		n.x -= x0;
		n.y -= y0;

		if(cin.fail())
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
		else
		{
			graph.addVertex(n);
		}
	}

	inputFile.close();

	return nodeCount;
}

int edgeFileToGraph(Graph<Node>& graph, const string &filePath)
{
	ifstream inputFile;
	if (ParsingHelper::openFileRead(inputFile, filePath) == -1)
		return -1;

	// nº de edges no ficheiro
	string temp;
	unsigned edgeCount;

	getline(inputFile, temp);
	if (!ParsingHelper::safeStoul(edgeCount, temp, 0, numeric_limits<unsigned>::max()))
		return -1;

	int id = 0;
	char c;
	while (inputFile.good())
	{
		getline(inputFile, temp);
		istringstream ss(temp);

		int orig;
		int dest;
		ss >> c;		// Skip '('
		ss >> orig; 	// Get origin ID
		ss >> c;		// Skip ','
		ss >> dest;   // Get destination ID

		if(cin.fail())
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
		else 
		{	
			Vertex<Node> *vO, *vD;
			Node n1, n2;
			vO = graph.findVertex(orig);
			vD = graph.findVertex(dest);
			if(vO != nullptr && vD != nullptr)
			{
				n1 = vO->getInfo();
				n2 = vD->getInfo();

				double dx = n2.x - n1.x;
				double dy = n2.y - n1.y;
				double weight = sqrt(dx * dx + dy * dy);
				graph.addEdge(n1, n2, weight, id++);
				graph.addEdge(n2, n1, weight, id++);
			}
			else
			{
				std::cerr << "addEdge failed" << endl;
			}
		}
	}

	inputFile.close();

	return id;
}

void tryDistanceBasedConnectionsForInaccessibleNodes(Graph<Node>& graph, vector<Vertex<Node>*> zeroOut, vector<Vertex<Node>*> zeroIn, int& edgeCount)
{
	for(auto& node : zeroOut)
	{
		double dst = INF;
		double temp;
		Vertex<Node>* closest = nullptr;
		Node thisNode = node->getInfo();


		for(auto& other : zeroIn)
		{
			Node otherNode = other->getInfo();

			if(thisNode == otherNode || other->getIngoing().size() != 0)
				continue;

			temp = sqrt( pow(otherNode.x - thisNode.x, 2) + pow(otherNode.y - thisNode.y, 2) );
			if(temp < dst && temp < 300)
			{
				dst = temp;
				closest = other;
			}
		}

		if(closest != nullptr)
		{
			graph.addEdge(thisNode, closest->getInfo(), dst, edgeCount++);
		}
	}
}

void removeDeadEnds(Graph<Node>& graph, queue<Vertex<Node>*> zeroOut)
{
	Vertex<Node>* current;
	Vertex<Node>* next;

	// Enquanto tiver nós na pilha
	while (!zeroOut.empty())
	{
		// remover do topo da pilha
		current = zeroOut.front();
		zeroOut.pop();

		cout << current->getInfo().id << endl;

		// saltar se tiver mais que uma ingoing edge
		if(current->getOutgoing().size() > 0)
			continue;

		for(auto& e : current->getIngoing())
		{
			next = e.getOrig();

			// adicionar à pilha as ingoing edges do nó
			zeroOut.push(next);

			graph.removeEdge(next->getInfo(), current->getInfo());
		}



		// apagar nó
		graph.removeVertex(current->getInfo());
	}
}

void checkInaccessibleNodes(Graph<Node>& graph, int& edgeCount, bool tryCorrection)
{
	vector<Vertex<Node>*> zeroOut;
	vector<Vertex<Node>*> zeroIn;
	vector<Vertex<Node>*> trash;
	for (auto &v : graph.getVertexSet())
	{
		if(v->getOutgoing().size() == 0 && v->getIngoing().size() != 0)
			zeroOut.push_back(v);
		else if(v->getIngoing().size() == 0 && v->getOutgoing().size() != 0)
			zeroIn.push_back(v);
		else if(v->getIngoing().size() == 0 && v->getOutgoing().size() == 0)
			trash.push_back(v);
	}

	cout << zeroOut.size() << " Inaccessible nodes found." << endl;
	cout << zeroIn.size() << " dead ends found." << endl;
	cout << trash.size() << " single nodes found. " << endl;

	for (auto& t : trash)
		graph.removeVertex(t->getInfo());



	if(tryCorrection)
		tryDistanceBasedConnectionsForInaccessibleNodes(graph, zeroOut, zeroIn, edgeCount);

	// queue<Vertex<Node>*> deadEnds;
	// for (const auto& e: zeroOut)
  	// 	deadEnds.push(e);

	// removeDeadEnds(graph, deadEnds);
	//cout << zeroOut.size() << " dead ends found." << endl;
}

void generateRandomPackages(unsigned amount, vector<Package>& packages, int seed, Graph<Node>& graph, int& edgeCount, bool doRandomSeed, bool printInfo)
{
	// Vetor duplicado para retirar aleatoriamente ids de nós
	size_t currShuffleID = 0;
	vector<int> shuffledIDs;
	for (auto &n : graph.getVertexSet())
		shuffledIDs.push_back(n->getInfo().id);

	// Baralhar vetor
    random_device rd;
	mt19937 g(seed);
	if(doRandomSeed)
    	g.seed(rd());
    shuffle(shuffledIDs.begin(), shuffledIDs.end(), g);

	unsigned packageID = 0;
	unsigned itrCount = 0;

	Node centro = graph.findVertex( CENTRO_APOIO )->getInfo();
	Vertex<Node>* vOrig;
	Vertex<Node>* vDest;
	Node origNode;
	Node destNode;

	vector<Node> path;

	while(packages.size() < amount)
	{
		// Não há mais pontos para testar
		if(currShuffleID >= shuffledIDs.size())
			break;

		// Incrementar contador de iterações
		itrCount++;

		// Calcular caminhos a partir do centro
		graph.dijkstraShortestPath( centro );

		// Procurar ponto de recolha
		vOrig = graph.findVertex(shuffledIDs.at(currShuffleID++));
		origNode = vOrig->getInfo();
		if(origNode.id != CENTRO_APOIO)
		{
			// Ver se é alcançável a partir do centro
			path = graph.getPath(vOrig->getInfo());

			if(path.empty())
				continue;

			// Calcular caminhos a partir do ponto de recolha encontrado
			graph.dijkstraShortestPath( origNode );
			while(currShuffleID < shuffledIDs.size())
			{
				// Incrementar contador de iterações
				itrCount++;

				// Procurar ponto de destino	
				vDest = graph.findVertex(shuffledIDs.at(currShuffleID++));
				destNode = vDest->getInfo();
				if(destNode.id != CENTRO_APOIO)
				{
					// Ver se é alcançável a partir do ponto de recolha
					path = graph.getPath(destNode);
					
					// Gerar encomenda
					if(!path.empty())
					{
						Package p(packageID, edgeCount++, vDest->getDist(), vOrig, vDest);
						packages.push_back(p);
						packageID++;
						break;
					}
				}
			}
		}
	}

	bool success = (packages.size() == amount);

	if(printInfo)
	{
		cout << "-------- Package Generator --------" << endl; 

		cout << itrCount << " iterations to finish" << endl;

		cout << ((success) ? "Success: " : "Fail: ") << packages.size() << " packages generated." << endl;
		cout << "-----------------------------------" << endl;
		for (auto& pack : packages)
			cout << pack.id << ": " << pack.orig->getInfo().id << " --> " << pack.dest->getInfo().id << endl;
		cout << "-----------------------------------" << endl;
	}
}

GraphViewer* drawGraph(const Graph<Node>& graph)
{
	// Criar grafo
	GraphViewer *gv = new GraphViewer(600, 600, false);
	gv->createWindow(800, 600);
	gv->defineVertexColor(NODE_DEFAULT_COLOR);
	gv->defineVertexSize(NODE_DEFAULT_SIZE);
	gv->defineEdgeColor(DARK_GRAY);
	gv->defineEdgeCurved(false);
	gv->defineEdgeDashed(true);

	// Desenhar nós
	for(auto& v : graph.getVertexSet())
	{
		Node node = v->getInfo();
		gv->addNode(node.id, node.x, -node.y);

		if(node.id == CENTRO_APOIO)
		{
			gv->setVertexColor(CENTRO_APOIO, YELLOW);
			gv->setVertexSize(CENTRO_APOIO, 60);
			gv->setVertexLabel(CENTRO_APOIO, "Centro de apoio");
		}

		if(v->getOutgoing().size() == 0 && v->getIngoing().size() != 0)
			gv->setVertexColor(node.id, RED);

		if(v->getIngoing().size() == 0 && v->getOutgoing().size() != 0)
			gv->setVertexColor(node.id, GREEN);
	}

	// Desenhar arestas
	for(auto& v : graph.getVertexSet())
		for (auto& e : v->getOutgoing())
			gv->addEdge(e.getEdgeID(), v->getInfo().id, e.getDest()->getInfo().id, EdgeType::DIRECTED);

	return gv;
}

bool addRoute(const Graph<Node>& graph, vector<Route>& routes, const Node& orig, const Node& dest, double dst)
{
	if(orig.id == dest.id)
		return false;

	Route r;

	vector<Node> path = graph.getPath( dest );

	r.ID = routes.size();
	r.totalDistance = dst;

	for(Node& n : path)
		r.nodeIDs.push_back(n.id);

	for (size_t i = 0; i < path.size() - 1; i++)
		r.edgeIDs.push_back( graph.getEdgeID(path.at(i), path.at(i+1)) );

	routes.push_back(r);

	return true;
}

void prepareDeliveryRouteForDisplay(Graph<Node>& graph, vector<Route>& routes, vector<Node>& deliveryRoute)
{
	Node orig = graph.findVertex(CENTRO_APOIO)->getInfo();
	Vertex<Node>* dest = graph.findVertex(deliveryRoute.at(0));
	graph.dijkstraShortestPath(orig);
	addRoute(graph, routes, orig, dest->getInfo(), dest->getDist());

	for (size_t i = 0; i < deliveryRoute.size() - 1; i++)
	{
		orig = deliveryRoute.at(i);
		graph.dijkstraShortestPath( orig );
		dest = graph.findVertex(deliveryRoute.at(i + 1));
		addRoute(graph, routes, orig, dest->getInfo(), dest->getDist());
	}
	orig = deliveryRoute.back();
	dest = graph.findVertex(CENTRO_APOIO);
	graph.dijkstraShortestPath(orig);
	addRoute(graph, routes, orig, dest->getInfo(), dest->getDist());
}

bool findSubOptimalDeliveryRoute(Graph<Node>& graph, vector<Node>& deliveryRoute, const vector<Package>& packages)
{
	vector<Vertex<Node>*> remainingPoints;
	map< Vertex<Node>*, Vertex<Node>* > pickUpDeliveryPairs;
	for(auto &p : packages)
	{
		// Criar o vetor com nodes de recolha
		remainingPoints.push_back( p.orig );
		// Criar mapa dos pontos de recolha para as respectivas entregas
		pickUpDeliveryPairs.insert( {p.orig, p.dest} );
	}

	Vertex<Node>* current;
	double dst2Curr;
	double shortestDst2Curr;
	bool isPickup;

 	// Definir ponto atual como centro de apoio
	current = graph.findVertex( CENTRO_APOIO );

	// Enquanto houver pontos a percorrer...
	while (remainingPoints.size() > 0)
	{
		shortestDst2Curr = INF;

		// Calcular caminhos a partir do ponto atual
		graph.dijkstraShortestPath(current->getInfo());

		// Procurar o ponto mais próximo do atual
		for(auto &node : remainingPoints)
		{
			dst2Curr = node->getDist();
			if(dst2Curr < shortestDst2Curr)
			{
				shortestDst2Curr = dst2Curr;
				current = node;
				isPickup = (pickUpDeliveryPairs.count(current) > 0);
			}
		}

		if(shortestDst2Curr == INF)
			break;

		// Saltar se o ponto atual for o centro de apoio
		if(current->getInfo().id == CENTRO_APOIO)
			continue;

		// Se ponto atual é de recolha, trocar pelo respectivo ponto de entrega
		if(isPickup)
			replace(remainingPoints.begin(), remainingPoints.end(), current, pickUpDeliveryPairs.at(current));
		// Se ponto atual é de entrega, remover
		else
			remainingPoints.erase(remove(remainingPoints.begin(), remainingPoints.end(), current), remainingPoints.end());

		// Adicionar ponto atual à rota final
		deliveryRoute.push_back(current->getInfo());
	}

	return (remainingPoints.size() == 0);
}

void testAverageRouteTime(Graph<Node>& graph, vector<Node>& deliveryRoute, vector<Package>& packages, 
							unsigned amount, int seed, int& edgeCount)
{
	packages.clear();

	generateRandomPackages(amount, packages, seed, graph, edgeCount, false, true);
	if(packages.size() == 0)
		return;

	cout << "ENTER to continue..." << endl << endl;
	getchar();


	cout << "-------- Delivery Route Finder --------" << endl;
	long long avg = 0;
	size_t iterations = 5;

	for (size_t i = 0; i < iterations; i++)
	{
		deliveryRoute.clear();
		auto start = chrono::steady_clock::now();
		bool success = findSubOptimalDeliveryRoute(graph, deliveryRoute, packages);
		auto end = chrono::steady_clock::now();

		avg += chrono::duration_cast<chrono::milliseconds>(end - start).count();

		cout << "Attempt: " << i+1 << " - " << ((success) ? "Success  |  " : "Fail  |  ");
		cout << "Elapsed time : " 
			<< chrono::duration_cast<chrono::milliseconds>(end - start).count()
			<< " ms" << endl;
	}
	cout << "Average time for " << iterations << " iterations: "
			<< avg / (long double)iterations
			<< " ms" << endl;
}

void testSingleRouteAndDraw(Graph<Node>& graph, vector<Node>& deliveryRoute, vector<Package>& packages, 
							unsigned amount, int seed, int& edgeCount)
{
	deliveryRoute.clear();
	packages.clear();

	// Gerar encomendas através de pontos aleatórios (Calcular rotas de cada encomenda)
	generateRandomPackages(amount, packages, seed, graph, edgeCount, false, true);
	if(packages.size() == 0)
		return;

	cout << "ENTER to continue..." << endl << endl;
	getchar();

	//Calcular rota para encomendas
	bool success = findSubOptimalDeliveryRoute(graph, deliveryRoute, packages);
	cout << "-------- Delivery Route Finder --------" << endl;
	cout << ((success) ? "Success" : "Fail") << endl;
	cout << "-----------------------------------" << endl;

	cout << "ENTER to continue..." << endl;
	getchar();

	// Preparar arestas ao long da rota para desenhar
	vector<Route> routes;
	prepareDeliveryRouteForDisplay(graph, routes, deliveryRoute);

	// Desenhar grafo
	GraphViewer *gv = drawGraph(graph);

	// Desenhar encomendas
	for(auto& p : packages)
		drawPackage(gv, p);

	// Desenhar rota
	for(auto& r : routes)
	{
		gv->setVertexColor(r.nodeIDs.at(0), YELLOW);
		gv->setVertexSize(r.nodeIDs.at(0), 100);
		drawRoute(gv, r);
		usleep(500000);
	}
}

long double testAverageRouteTimeWithRandomPackages(Graph<Node>& graph, vector<Node>& deliveryRoute, vector<Package>& packages, 
							unsigned amount, int seed, int& edgeCount, bool printInfo)
{
	if(printInfo)
		cout << "-------- Delivery Route Finder (random packages) --------" << endl;
	long long avg2 = 0;

	for (int i = 0; i < seed; i++)
	{
		packages.clear();

		generateRandomPackages(amount, packages, seed, graph, edgeCount, true, false);
		if(packages.size() == 0)
			return -1;

		long long avg = 0;
		size_t iterations = 5;
		bool success = false;

		for (size_t i = 0; i < iterations; i++)
		{
			deliveryRoute.clear();
			auto start = chrono::steady_clock::now();
			success = findSubOptimalDeliveryRoute(graph, deliveryRoute, packages);
			auto end = chrono::steady_clock::now();

			avg += chrono::duration_cast<chrono::milliseconds>(end - start).count();
		}

		avg2 += avg / (long double)iterations;

		if(printInfo)
		{
			cout << "Attempt: " << i+1 << " - " << ((success) ? "Success  |  " : "Fail  |  ");
			cout << "Average time for " << iterations << " iterations: "
					<< avg / (long double)iterations
					<< " ms" << endl;		
		}
	}
	avg2 = avg2 / (long double)seed;

	if(printInfo)
		cout << "Average time for " << seed << " iterations: " << avg2 << " ms" << endl;		

	return avg2;
}

int main(int argc, char* argv[])
{
	Graph<Node> myGraph;
	int seed;
	int nodeCount;
	int edgeCount;
	unsigned packageAmount;

	if(argc == 5)
	{
		seed = atoi(argv[1]);
		packageAmount = atoi(argv[2]);
		if( (nodeCount = nodeFileToGraph(myGraph, string(argv[3]))) == -1 )
		{
			std::cerr << "Failed to read node file: " << string(argv[2]) << endl;
			return -1;
		}

		if( (edgeCount = edgeFileToGraph(myGraph, string(argv[4]))) == -1 )
		{
			std::cerr << "Failed to read edge file: " << string(argv[3]) << endl;
			return -1;
		}
	}
	else
	{
		std::cerr << "Wrong usage: [seed int] [package count uint] [node file path] [edge file path]" << endl;
		return -1;
	}

	cout << nodeCount << " nodes read." << endl;
	cout << edgeCount << " edges read." << endl;

	// Verificar nós inatingíveis/sem saída -> Tentar ligá-los entre si
	checkInaccessibleNodes(myGraph, edgeCount, false);

	cout << "ENTER to continue..." << endl << endl;
	getchar();

	vector<Package> randomPackages;
	vector<Node> deliveryRoute;

	
	// AVERAGE ROUTE TIMES WITH RANDOM PACKAGES
	//testAverageRouteTimeWithRandomPackages(myGraph, deliveryRoute, randomPackages, packageAmount, seed, edgeCount, true);

	// // AVERAGE ROUTE TIMES
	//testAverageRouteTime(myGraph, deliveryRoute, randomPackages, packageAmount, seed, edgeCount);

	// // SINGLE ROUTE + DRAWING
	testSingleRouteAndDraw(myGraph, deliveryRoute, randomPackages, packageAmount, seed, edgeCount);

	return 0;
}
