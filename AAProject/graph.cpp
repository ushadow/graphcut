#include <iostream>
#include <queue>
#include <fstream>
#include <string>

#include "graph.h"

using namespace std;

const string Graph::s_ALGO_NAMES[] = {"DFS", "BFS", "MC", "DINIC"};

Graph::Graph(int numNodes)
{
	this->m_numNodes = numNodes;
	m_nodes.resize(m_numNodes);
}

Graph::~Graph()
{
	m_nodes.clear();
}

void Graph::addEdge(int startPoint, int endPoint, int cap)
{
	if(startPoint < m_numNodes)
	{
		m_nodes[startPoint].neighbors[endPoint] = cap;
	}
	else cerr << "Node index is output bound!" << endl;
}

void Graph::minCut(Graph::Algo algo)
{
	int maxflow;
	switch(algo)
	{
	case Graph::DFS:
	case Graph::BFS:
	case Graph::MC:
		maxflow = fordFulkerson(algo);
		break;
	case Graph::DINIC:
		maxflow = dinic();
		break;
	default:
		break;
	}

	cout<< s_ALGO_NAMES[algo] << ": maxflow = " << maxflow << endl;

	color(m_source);
}

int Graph::fordFulkerson(Graph::Algo algo)
{
	int maxflow = 0;
	int numIters = 0;

	while(true)
	{
		int pathCapacity = findPath(algo);

		//no more augmenting path
		if(pathCapacity == 0)
			break;

		maxflow += pathCapacity;
		numIters++;
	}

	cout<<"numIters = " << numIters << endl;
	return maxflow;
}

int Graph::dinic()
{
	int flow = 0;
	int numIters = 0;

	int *q, *prev;

	q = new int[m_numNodes];
	prev = new int[m_numNodes];

	while(true)
	{
		memset(prev,-1,m_numNodes*sizeof(int));	
		int qf = 0, qb = 0;
		prev[q[qb++]=m_source] = -2;
		while(qb > qf && prev[m_sink] == -1)
		{
			int u = q[qf++], v;
			for(map<int,int>::iterator it = m_nodes[u].neighbors.begin(); it != m_nodes[u].neighbors.end(); it++)
				if(prev[v = (*it).first] == -1 && (*it).second)
					prev[q[qb++] = v ] = u;
		}
		if(prev[m_sink] == -1) break; //cannot find path anymore, we're done
		//cout<<"found a path"<<endl;
		for(int z = 0; z < m_numNodes; z++)
			if(m_nodes[z].neighbors[m_sink] && prev[z] != -1)
			{
				int bot = m_nodes[z].neighbors[m_sink];
				for(int v = z, u = prev[v]; u >= 0; v = u, u = prev[v])
					bot = bot < m_nodes[u].neighbors[v]? bot : m_nodes[u].neighbors[v];
				if(!bot) continue;
				
				m_nodes[z].neighbors[m_sink] -= bot;
				m_nodes[m_sink].neighbors[z] += bot;
				for(int v = z, u = prev[v]; u >= 0; v = u, u = prev[v])
				{
					m_nodes[u].neighbors[v] -= bot;
					m_nodes[v].neighbors[u] += bot;
				}
				flow += bot;
			}
		numIters++;
	}

	delete q;
	delete prev;

	cout<<"numIters = " << numIters << endl;
	return flow;
}

void Graph::printColor()
{
	cout<<"Color 1:"<<endl;
	for(int i = 0; i < m_numNodes; i++)
		if(m_nodes[i].color == 1)
			cout << i << " ";
	cout<<endl;
}

void Graph::saveColor(string outputFile)
{
	ofstream fout;
	fout.open(outputFile.c_str(),ios::binary);

	char* buf;

	int size = m_numNodes - 2;
	buf = new char[size];

	for (int i = 0; i < size; i++)
		buf[i] = m_nodes[i].color;

	fout.write(buf,size);
}

int Graph::findPath(Graph::Algo algo)
{
	int flow = 0;

	switch(algo)
	{
	case Graph::DFS:
		flow = dfs();
		break;
	case Graph::BFS:
		flow = bfs();
		break;
	case Graph::MC:
		flow = pfs();
		break;
	default:
		break;
	}

	return flow;
}

void Graph::setSourceSink(int source, int sink)
{
	this->m_source = source;
	this->m_sink = sink;
	m_nodes[m_source].color = 1;
}

int Graph::bfs()
{
	queue<int> queue;
	vector<int> from(m_numNodes,-1);

	queue.push(m_source);

	//mark source as visited, the node is unvisited if from[node]==-1
	from[m_source] = -2;

	//find one path
	while(!queue.empty())
	{
		bool found = false;
		int node = queue.front();

		queue.pop();
		for(map<int,int>::iterator it = m_nodes[node].neighbors.begin(); it != m_nodes[node].neighbors.end(); it++)
		{
			int next = (*it).first;
			int cap = (*it).second;
			if( from[next]==-1 && cap > 0 )
			{
				queue.push(next);
				from[next] = node;
				if(next == m_sink)
				{
					found = true;
					break;
				}
			}
		}

		if(found)
			break;
	}

	//compute path capacity
	int curNode = m_sink;

	int path_cap = INT_MAX;

	while(from[curNode] > -1)
	{
		int prev = from[curNode];
		path_cap = min(path_cap, m_nodes[prev].neighbors[curNode]);
		curNode = prev;
	}

	curNode = m_sink;

	//update the residual network
	while(from[curNode] > -1)
	{
		int prev = from[curNode];
		m_nodes[prev].neighbors[curNode] -= path_cap;
		m_nodes[curNode].neighbors[prev] += path_cap;
		curNode = prev;
	}

	if(path_cap == INT_MAX)
		return 0;

	return path_cap;
}

void Graph::color(int node)
{
	m_nodes[node].color = 1;
	for(map<int,int>::iterator it = m_nodes[node].neighbors.begin(); it != m_nodes[node].neighbors.end(); it++)
	{
		int neighbor = (*it).first;
		if ((*it).second > 0 && m_nodes[neighbor].color == 0)
			color(neighbor);
	}
}

int Graph::dfs()
{
	vector<int> from(m_numNodes,-1);
	return dfsHelper(m_source, from,INT_MAX);
}

int Graph::dfsHelper(int node, vector<int>& from, int cap)
{
	if(node == m_sink)
		return cap;
	
	int bot = 0;

	for(map<int,int>::iterator it = m_nodes[node].neighbors.begin(); it != m_nodes[node].neighbors.end(); it++)
	{
		int next = (*it).first;
		if ((*it).second > 0  && from[next]==-1)
		{
			cap = min(cap,(*it).second);
			from[next] = node;
			bot = dfsHelper(next,from,cap);
			
			if(bot>0)
			{
				m_nodes[node].neighbors[next] -= bot;
				m_nodes[next].neighbors[node] += bot;
				break;
			}
		}
	}
	
	return bot;
}

int Graph::pfs()
{
	priority_queue<ExtendedNode> pq;
	vector<int> from(m_numNodes,-1);

	ExtendedNode node = {m_source, INT_MAX, -2};
	pq.push(node);
	
	int path_cap = 0;

	while(!pq.empty())
	{
		ExtendedNode aux = pq.top();
		pq.pop();

		int current = aux.node;
		int cost = aux.cost;

		from[current] = aux.from;

		if(current == m_sink)
		{
			path_cap = cost;
			break;
		}

		for(map<int,int>::iterator it = m_nodes[current].neighbors.begin(); it != m_nodes[current].neighbors.end(); it++)
		{
			int next = (*it).first;
			if( from[next] == -1 && (*it).second > 0)
			{
				int new_cost = min(cost, (*it).second);
				node.node = next;
				node.cost = new_cost;
				node.from = current;
				pq.push(node);
			}
		}
	}
	int current = m_sink;

	while(from[current]>-1)
	{
		//cout<<"curent = "<<current<<endl;
		int prev = from[current];
		m_nodes[prev].neighbors[current] -= path_cap;
		m_nodes[current].neighbors[prev] += path_cap;
		current = prev;
	}

	return path_cap;
}

