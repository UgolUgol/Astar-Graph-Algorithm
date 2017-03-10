#ifndef _GRAPH_H
#define _GRAPH_H

#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <fstream>
#include <cstdint>
#include <cstring>
using namespace std;

#define UINT reinterpret_cast<char*>
#define COORD reinterpret_cast<char*>
#define DOUBLE reinterpret_cast<char*>
#define STRUCT reinterpret_cast<char*>

/*-------------------------------------*/
const double Radius = 6371.;
const int Infinity = -1;

template <class A, class B> struct TEdge
{
	A first;
	B second;

	TEdge(){}
	TEdge(A p1, B p2)
	{
		first = p1;
		second = p2;
	}
};


struct TGraphComponent
{
	typedef TEdge<unsigned char, size_t> TParameters;
	TParameters Parameters;
	vector<TEdge<uint64_t, double> > Neighbours;

};

struct TNodes
{
	vector<TGraphComponent> Node;
	vector<TEdge<double, double> > Coordinates;
	uint64_t Edge;
	uint64_t NodesCount;
};

struct TLists
{
	struct compare
	{
		bool operator() (TEdge<uint64_t, double>& a, TEdge<uint64_t, double>& b)
		{
			return (a.second > b.second);
		}
	};

	typedef vector<bool> ClosedNodes;
	typedef priority_queue<TEdge<uint64_t, double>, vector<TEdge<uint64_t, double> >, compare> OpenNodes;

	ClosedNodes Closed;
	OpenNodes Opened;			// opened nodes
	vector<double> Watched;		// means nodes thats has been already added to Opened
};



class TGraph
{
protected:
    TNodes Nodes;
	//TEdgeWeight Weight;
	//TLists Lists;
private:
	vector<uint64_t> Path;		// links on parent;
	double CoveredDistance;		// distance
	double* Heuristic;
public:
	TGraph();

	double DistanceFromStart(uint64_t to, uint64_t parent_node);

	bool isAvaliableNode(uint64_t node, TLists& Lists);

	void SetDefaultParameters(uint64_t start, TLists& Lists);
	void ClearData();
	void RepairPath(uint64_t start, uint64_t goal,std::ostream& output);
	void AddSurroundingsNodes(uint64_t parent_node, uint64_t goal, TLists& Lists);

	void FindPath(uint64_t start, uint64_t goal, std::ostream& output, bool quiteMode = true);
	void CountHeuristic(uint64_t from, uint64_t to);

	friend std::istream& operator>>(std::istream&, TGraph&);
	double HaverSin(uint64_t, uint64_t);

	~TGraph();
};



class TPreprocessGraph : protected TGraph
{
	ifstream Input;
	ofstream Output;
	bool* Watched;
protected:
	void ReadBinaryGraph();
	void DeepingSearch(uint64_t start, unsigned char& component);
	void FindComponents();
	void WritePreprocessGraph(const char* output);
public:
	void Preprocess(const char* filename, const char* output);

	~TPreprocessGraph();
};



#endif
