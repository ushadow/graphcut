#include<stdlib.h>
#include<iostream>
#include<vector>
#include<fstream>
#include<sstream>
#include<string>
#include<sys/time.h>

#include "graph.h"
#include "Timer.h"

using namespace std;

const Graph::Algo ALGO = Graph::MC;
const int BETA = 11;
const string INPUT_FILE_PREFIX = "I:\\YingYin\\MITCourses\\6.854AA\\data\\lambda";
//const string INPUT_FILE = "/afs/csail.mit.edu/u/y/yingyin/YingYin/MITCourses/6.854AA/data/lambda.dat";

const string OUTPUT_FILE_PREFIX = "I:\\YingYin\\MITCourses\\6.854AA\\data\\";
//const string OUTPUT_FILE = "/afs/csail.mit.edu/u/y/yingyin/YingYin/MITCourses/6.854AA/data/output.dat";

const int H = 176;
const int W = 240;

void timer (struct timespec *ts)
{
  //struct rusage r;

  //getrusage(0, &r);
  //return (float)(r.ru_utime.tv_sec * 1000000 +r.ru_utime.tv_usec);

	clock_gettime(CLOCK_REALTIME, ts);

}

struct timespec diff(struct timespec start, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

int main(void)
{
	//Graph graph(7);

	//graph.addEdge(1,0,3);
	//graph.addEdge(0,1,0);
	//graph.addEdge(1,2,1);
	//graph.addEdge(2,1,0);
	//graph.addEdge(2,3,5);
	//graph.addEdge(3,2,0);
	//graph.addEdge(3,4,2);
	//graph.addEdge(4,3,0);
	//graph.addEdge(2,5,4);
	//graph.addEdge(5,2,0);
	//graph.addEdge(5,6,2);
	//graph.addEdge(6,5,0);
	//graph.addEdge(6,4,3);
	//graph.addEdge(4,6,0);
	//graph.addEdge(0,3,3);
	//graph.addEdge(3,0,0);

	//graph.setSourceSink(1,4);

	//graph.minCut(ALGO);
	//graph.printColor();

	int h, w, beta, algo, scale;
	cin>>beta>>h>>w>>scale>>algo;
	cout<<"beta = " << beta<<" h = " << h<<" w = "<<w<<" algo = "<<Graph::s_ALGO_NAMES[algo]<<endl;
	char* lambda;
	ifstream fin;
	int dx[8] = {1,1,0,-1,-1,-1,0,1};
	int dy[8] = {0,-1,-1,-1,0,1,1,1};

	lambda = new char[h*w];
	ostringstream inputFile;
	inputFile<< INPUT_FILE_PREFIX<<scale<<".dat";
	cout<<"input file = "<<inputFile.str()<<endl;
	fin.open(inputFile.str().c_str(), ios::binary);
	fin.read(lambda, h*w);
	cout<<"read file"<<endl;

	int numNodes = h * w + 2;
	int s = h * w;
	int t = h * w + 1;

	Graph graph(numNodes);

	graph.setSourceSink(s, t);

	for(int i = 0; i < h * w; i++)
	{
		int cap = lambda[i];
		if(cap>0)
		{
			graph.addEdge(s, i, cap);
			graph.addEdge(i,s,0);
		}
		else
		{
			graph.addEdge(i,t,-cap);
			graph.addEdge(t,i,0);
		}
	}

	for(int y = 0; y < h; y++)
		for(int x = 0; x < w; x++)
		{
			for(int k = 0; k < 8; k++)
			{
				int nx = x + dx[k];
				int ny = y + dy[k];
				if( nx>=0 && nx<w && ny>=0 && ny<h)
					graph.addEdge(y*w+x, ny*w+nx, beta);
			}
		}

	cout<<"constructed graph"<<endl;
	Timer timer("maxflow");
	timer.tic();
	graph.minCut((Graph::Algo)algo);
	timer.tocln("finished mincut");

	ostringstream outputFile;
	outputFile << OUTPUT_FILE_PREFIX << Graph::s_ALGO_NAMES[algo] << beta << ".dat";
	cout<<"Output file = " << outputFile.str() << endl;
	graph.saveColor(outputFile.str());

	return 0;
}



