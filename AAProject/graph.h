#ifndef _H_GRAPH_
#define _H_GRAPH_

#include<stdlib.h>
#include<map>
#include<vector>
#include<string>

using namespace std;

struct Node
{
	int color;
	map<int, int> neighbors;

	Node():color(0){};
};

struct ExtendedNode
{
	int node, cost, from;

	bool operator<(const ExtendedNode& node2) const
	{
		return this->cost > node2.cost;
	}
};

class Graph
{
public:
	enum Algo {DFS, BFS, MC, DINIC, TOTAL_NUM}; 
	static const string s_ALGO_NAMES[];

	Graph(int numNodes);

	~Graph();

	void addEdge(int startPoint, int endPoint, int cap);

	void minCut(Algo algo);

	void setSourceSink(int source, int sink);

	void printColor();

	void saveColor(string outputFile);

private:
	vector<Node> m_nodes;
	int m_numNodes, m_source, m_sink;

	int fordFulkerson(Algo algo);

	int dinic();

	int findPath(Algo algo);

	int bfs();

	int pfs();

	int dfs();

	int dfsHelper(int node, vector<int>& from, int cap);

	//Effects: color the nodes in the S cut to white(1)
	void color(int node);
};

#endif //_H_GRAPH_

