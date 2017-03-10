#include "Graph.h"
#include <ctime>
#include <stack>
#include <cstdlib>
#include "Stack.h"

// first - longtitude
// second - lattitude


TGraph::TGraph()
{
	Nodes.Edge = 0;
	Nodes.NodesCount = 0;
	CoveredDistance = 0;
	Heuristic = nullptr;
}


double TGraph::HaverSin(uint64_t u, uint64_t v)
{
	double toRad = 0.0174533;

	return (2 * asin(sqrt(
		pow(sin((toRad * Nodes.Coordinates[v].second - toRad * Nodes.Coordinates[u].second) / 2), 2) +
		cos(toRad * Nodes.Coordinates[u].second) * cos(toRad * Nodes.Coordinates[v].second) *
		pow(sin((toRad * Nodes.Coordinates[v].first - toRad * Nodes.Coordinates[u].first) / 2), 2)
		)));
}

std::istream& operator>>(std::istream& input, TGraph& in)
{
	input.read(UINT(&in.Nodes.NodesCount), sizeof(uint64_t));
	input.read(UINT(&in.Nodes.Edge), sizeof(uint64_t));

	in.Nodes.Node.resize(in.Nodes.NodesCount);
	in.Nodes.Coordinates.resize(in.Nodes.NodesCount);
	in.Heuristic = (double*)malloc(in.Nodes.NodesCount * sizeof(double));
	input.read(COORD( in.Nodes.Coordinates.data() ), in.Nodes.NodesCount * sizeof(TEdge<double, double>));

	for (uint64_t i = 0; i < in.Nodes.NodesCount; ++i)
	{
		input.read(STRUCT(&in.Nodes.Node[i].Parameters), sizeof(TEdge<unsigned char, size_t>));
		in.Nodes.Node[i].Neighbours.resize(in.Nodes.Node[i].Parameters.second);
		input.read(UINT((in.Nodes.Node[i].Neighbours.data())),
			sizeof(TEdge<uint64_t, double>) * in.Nodes.Node[i].Parameters.second);

	}
	return input;
}


double TGraph::DistanceFromStart(uint64_t to, uint64_t parent_node)
{
	return this->Nodes.Node[parent_node].Neighbours[to].second + this->CoveredDistance;
}


bool TGraph::isAvaliableNode(uint64_t node, TLists& Lists)
{
	if (Lists.Closed[node] == true)
	{
		return false;
	}
	else
	{
		return true;
	}
}


void TGraph::CountHeuristic(uint64_t from, uint64_t to)
{
	Heuristic[from] = Radius * HaverSin(from, to);
}

void TGraph::AddSurroundingsNodes(uint64_t parent_node, uint64_t goal, TLists& Lists)
{
	for (uint64_t i = 0; i < this->Nodes.Node[parent_node].Neighbours.size(); ++i)
	{
		uint64_t to = this->Nodes.Node[parent_node].Neighbours[i].first;
		if (!Lists.Watched[to])
		{
			/*if this node has been already opened -
			the heuristic for it also was found some times ago
			and this clause doesn't work*/
			CountHeuristic(to, goal);
		}
		double priority = DistanceFromStart(i, parent_node) + Heuristic[to];
		if (Lists.Watched[to] && Lists.Watched[to] <= priority)
		{
			/*in case we find node
			which has been already opened and it have
			priority bigger than another one - don't change anything*/
			continue;
		}
		if (isAvaliableNode(to, Lists))
		{
			Lists.Opened.emplace(TEdge<uint64_t,double>(to, priority));
			Lists.Watched[to] = priority;							// min priority for current neighbour node
			this->Path[to] = parent_node;
			//std::cout << "We added " << to+1 << " with priority " << priority << std::endl;
		}
	}
	//system("pause");
	//cout << std::endl;
}

void TGraph::FindPath(uint64_t start, uint64_t goal,std::ostream& output, bool quiteMode)
{
	--start;
	--goal;

	bool separate = (Nodes.Node[start].Parameters.first != Nodes.Node[goal].Parameters.first);
	if(separate)
	{
		output.write((char*)(&Infinity), sizeof(const int));
	}

	else {
		TLists Lists;
		uint64_t current = -1;
		SetDefaultParameters(start, Lists);


		while (!Lists.Opened.empty())
		{
			current = Lists.Opened.top().first;
			double priority = Lists.Opened.top().second;
			Lists.Opened.pop();
			Lists.Closed[current] = true;
			this->CoveredDistance = (current == start ? 0 : priority - Heuristic[current]);
			//	std::cout << "now at " << current+1 << " " << "with distance " << CoveredDistance << std::endl;;
			if (current == goal)
			{
				break;
			}
			AddSurroundingsNodes(current, goal, Lists);
		}
		CoveredDistance *= 1000;
		output.write(DOUBLE(&CoveredDistance), sizeof(double));
		if (!quiteMode)
		{
			RepairPath(start, goal, output);
		}
	}
	return;
}

void TGraph::SetDefaultParameters(uint64_t start, TLists& Lists)
{
	Lists.Closed.resize(this->Nodes.NodesCount, false);
	Lists.Opened.push(TEdge<uint64_t, double>(start, 0));
	Lists.Watched.resize(this->Nodes.NodesCount, 0.0);

	this->Path.resize(this->Nodes.NodesCount);
}

void TGraph::ClearData()
{

}

void  TGraph::RepairPath(uint64_t start, uint64_t goal, std::ostream& output)
{
	uint64_t totalNodes = 1;
	for (uint64_t i = goal; i != start; i = this->Path[i])
	{
		totalNodes++;
	}
	output.write(UINT(&totalNodes), sizeof(uint64_t));
	vector<uint64_t> way(totalNodes);
    for (int i = goal,  j = totalNodes - 1; i != start; i = this->Path[i], --j)
	{
		way[j] = i + 1;
	}
	way[0] = start + 1;
	output.write(UINT(way.data()), totalNodes * sizeof(uint64_t));
}

TGraph::~TGraph()
{
    free(this->Heuristic);
}
/*-------Preprocessing-------------------*/


void TPreprocessGraph::Preprocess(const char* filename, const char* output)
{
	Input.open(filename, std::ios::binary);
	ReadBinaryGraph();;
	FindComponents();
	WritePreprocessGraph(output);
	Input.close();
}

void TPreprocessGraph::ReadBinaryGraph()
{
	Input.read(UINT(&Nodes.NodesCount), sizeof(uint64_t));
	Input.read(UINT(&Nodes.Edge), sizeof(uint64_t));

	uint64_t idx = 0;
	Nodes.Node.resize(Nodes.NodesCount);
	Nodes.Coordinates.resize(Nodes.NodesCount);
	Input.read(COORD(Nodes.Coordinates.data()), Nodes.NodesCount * sizeof(TEdge<double, double>));

	TEdge<uint64_t, uint64_t>* edges = new TEdge<uint64_t, uint64_t>[Nodes.Edge];
	Input.read(UINT(edges), sizeof(TEdge<uint64_t, uint64_t>) * Nodes.Edge);

	double distance = 0;
	TEdge<uint64_t, double> neighbour;
	while (idx < Nodes.Edge)
	{
		--edges[idx].first;
		--edges[idx].second;
		distance = Radius *  HaverSin(edges[idx].first, edges[idx].second);
		neighbour.first = edges[idx].second;
		neighbour.second = distance;
		Nodes.Node[edges[idx].first].Neighbours.push_back(neighbour);
		++idx;
	}
	for (uint64_t i = 0; i < Nodes.NodesCount; ++i)
	{
		Nodes.Node[i].Parameters.second = Nodes.Node[i].Neighbours.size();
	}
	delete[] edges;
}


void TPreprocessGraph::FindComponents()
{
    this->Watched = new bool[Nodes.NodesCount];
	for (uint64_t i = 0; i < Nodes.NodesCount; ++i)
	{
		Watched[i] = false;
	}

	unsigned char comp = 'a';
	for (uint64_t i = 0; i < Nodes.NodesCount; ++i)
	{
		if (!Watched[i])
		{
			Watched[i] = true;
			DeepingSearch(i, comp);
			++comp;
		}
	}
}


void TPreprocessGraph::DeepingSearch(uint64_t start, unsigned char& component)
{
	bool* closed = new bool[Nodes.NodesCount];
	for (uint64_t i = 0; i < Nodes.NodesCount; ++i)
	{
		closed[i] = false;
	}
	TStack<uint64_t> s;
	s.Push(start);
	closed[start] = true;
	while (!s.Empty())
	{
		uint64_t current = s.Top();
		Nodes.Node[current].Parameters.first = component;
		Watched[current] = true;
		s.Pop();
		for (uint64_t i = 0; i < Nodes.Node[current].Neighbours.size(); ++i)
		{
			uint64_t to = Nodes.Node[current].Neighbours[i].first;
			if (!closed[to])			{
				closed[to] = true;
				s.Push(to);
			}
		}
	}
	delete[] closed;
}


void TPreprocessGraph::WritePreprocessGraph(const char* output)
{
	Output.open(output, ios::binary);
	Output.write(UINT(&Nodes.NodesCount), sizeof(uint64_t));
	Output.write(UINT(&Nodes.Edge), sizeof(uint64_t));
    Output.write(COORD(Nodes.Coordinates.data()), Nodes.NodesCount * sizeof(TEdge<double, double>));
	for (uint64_t i = 0; i < Nodes.NodesCount; ++i)
	{
		Output.write(STRUCT(&Nodes.Node[i].Parameters), sizeof(TEdge<unsigned char, size_t>));
		Output.write(STRUCT((Nodes.Node[i].Neighbours.data())), sizeof(TEdge<uint64_t, double>)  *
			Nodes.Node[i].Parameters.second);
	}
	Output.close();
}
TPreprocessGraph::~TPreprocessGraph()
{
    delete[] this->Watched;
}
