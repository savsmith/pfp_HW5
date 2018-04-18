#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <atomic>
#include <pthread.h> 
#include <vector>
#include "ittnotify.h" // header for VTune calls

#include "graph.h"

#define INT_MAX std::numeric_limits<int>::max()
#define MAX_THREADS 512 

enum SourceNode {
	other = 4,
	rmat = 0,
	roadFLA = 316606,
	roadNY = 140960
};

class parallelBellmanFord {
	graph &g;
	int numThreads;
	SourceNode src;
	std::atomic<int>* distances;

	/* THREAD VARS */
	pthread_t handles[MAX_THREADS];
	int shortNames[MAX_THREADS];
	std::vector<graph::node_t> nodes[MAX_THREADS];

private:
	
	bool converged(std::atomic<int>* distances, int dists[]){
		//std::cout << "distances: " <<std::endl;
		//printarray(distances, g.size_nodes());
		//std::cout << "dists: " <<std::endl;
		//printarray(dists, g.size_nodes());
		__itt_resume();
		for(int n = g.begin(); n < g.end(); n++) {
			if (distances[n].load() != dists[n])
				return false; 
		}
		__itt_pause();
		return true;
	}

	void dists(std::atomic<int>* distances, int* container) {
		for(int n = g.begin(); n < g.end(); n++) {
			container[n] = distances[n].load();
		}
	}
	void printarray(int* array, int size){
		for(int i = 0; i < size; i++){
			std::cout << array[i] << std::endl;
		}
	}

	void initialize() {
		for(int n = g.begin(); n < g.end(); n++) {
			distances[n].store(INT_MAX);
		}
		distances[src].store(0);

		for(int i = 0; i < numThreads; i++){
			nodes[i] = std::vector<int>();
		}

		for(auto i = g.begin(); i < g.size_nodes(); i+=1){
			(nodes[i%numThreads]).push_back(i);
		}
	}
	void setDist(std::atomic<int> &orig, int next) {
		auto current = orig.load();
		while (!orig.compare_exchange_weak(current, next));
	} 

public:
	parallelBellmanFord(graph &g, int t): g(g), numThreads(t) { 
		distances = new std::atomic<int>[g.size_nodes()];

		string type = g.getGraphType();
		if (type.compare("rmat") == 0) {
			src = rmat;
			//cout << "type assigned to rmat" << endl;
		} else if (type.compare("roadFLA") == 0) {
			src = roadFLA;
			//cout << "type assigned to roadFLA" << endl;
		} else if (type.compare("roadNY") == 0) {
			src = roadNY;
			//cout << "type assigned to roadNY" << endl;
		} else if (type.compare("other") == 0) {
			cout << "type assigned to other" << endl;
			src = other;
		}
	}

	void relax(int myId) {
		__itt_resume();
  		for(auto& u : nodes[myId%numThreads]){
			for(auto e = g.edge_begin(u); e < g.edge_end(u); e++) {
				int v = g.get_edge_dst(e);
				int weight = g.get_edge_data(e);

				if(distances[u].load() != INT_MAX && distances[u].load() + weight < distances[v].load()) {
					int current = distances[v].load();
					int next = distances[u].load() + weight;
					while (!distances[v].compare_exchange_weak(current, next));
				}
			}
		}
		__itt_pause();
  		pthread_exit(NULL);
	}

	static void* relax_wrapper(void* object)
	{
		static std::atomic<int> myId {0};
		reinterpret_cast<parallelBellmanFord*>(object)->relax(myId++);
	}

	void bellmanFord(){
		initialize();

		int step = g.size_nodes()/numThreads;
		int prev[g.size_nodes()];
		for(int roundNum = 1; roundNum < g.size_nodes(); roundNum++){
			dists(distances, prev);
			
			for(int i = 0; i < numThreads; i+=1) {
				int rc = pthread_create(&handles[i], NULL, relax_wrapper, this);
			    if (rc) {
			      std::cout << "Error:unable to create thread," << rc << std::endl;
			      exit(-1);
			    }
			}

			for(int i=0; i < numThreads; i++){
	    		pthread_join(handles[i], NULL);
	  		}
	  		if(converged(distances, prev)){
	  			//std::cout << "Exited due to convergence" << std::endl;
	  			break;
	  		} else {
	  			//std::cout << "Graph did not converge" << std::endl;
	  		}
	  	}
  		//printGraphDistances();
	}

	void printGraphDistances() {
		//std::cout << "Distances" << std::endl;
		for (auto n = g.begin(); n < g.end(); n++) {
			if (distances[n] == INT_MAX) {
				std::cout << n+1 << " INF" << std::endl;
			} else {
				std::cout << n+1 << " " << distances[n] << std::endl;
			}
		}
		cout << endl;
	}

};

int main (int argc, char *argv[]) {
	// Ensure right number of arguments being used
	if (argc < 2) {
	    std::cerr << "Usage: " << argv[0] << " <input.dimacs>\n";
	    return 0;
  	}

	// std::cout << "Starting Program" << std::endl;
	
	graph g; // Construct graph g from input
	if(!g.construct_from_dimacs(argv[1])) {
		return 0;
	}
	std::cout << "Graph Name: " << g.getName() << std::endl;
	
	parallelBellmanFord bf(g, atoi(argv[2]));

	bf.bellmanFord();
	return 0;
}
