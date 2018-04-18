#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <limits>
#include "ittnotify.h" // header for VTune calls

#include "graph.h"

#define INT_MAX std::numeric_limits<int>::max()

enum SourceNode {
	other = 4,
	rmat = 0,
	roadFLA = 316606,
	roadNY = 140960
};

class serialBellmanFord {
	graph &g;
	SourceNode src;
	int *distances;

private:
	void initialize() {
		for(int n = g.begin(); n < g.end(); n++) {
			distances[n] = INT_MAX;
		}
		distances[src] = 0;
		//printGraphDistances();
	}
	bool converged(int distances[], int dists[]){
		//std::cout << "distances: " <<std::endl;
		//printarray(distances, g.size_nodes());
		//std::cout << "dists: " <<std::endl;
		//printarray(dists, g.size_nodes());
		for(int n = g.begin(); n < g.end(); n++) {
			if (distances[n] != dists[n])
				return false; 
		}
		return true;
	}

	void dists(int* distances, int* container) {
		for(int n = g.begin(); n < g.end(); n++) {
			container[n] = distances[n];
		}
	}
	void printarray(int* array, int size){
		for(int i = 0; i < size; i++){
			std::cout << array[i] << std::endl;
		}
	}

public:
	serialBellmanFord(graph &g): g(g) { 
		distances = new int[g.size_nodes()];

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

	void bellmanFord() {
  		initialize();
  		// N - 1 iterations...
		int prev[g.size_nodes()];
		__itt_resume();
  		for(auto i = 0; i < g.size_nodes() - 1; i++) {
  			dists(distances, prev);
	  		for(auto u = g.begin(); u < g.end(); u++) {
	  			// For each outgoing edge from source u...
	  			for(auto e = g.edge_begin(u); e < g.edge_end(u); e++) {
	  				int v = g.get_edge_dst(e);
	  				int weight = g.get_edge_data(e);

	  				if(distances[u] != INT_MAX && distances[u] + weight < distances[v]) {
	  					distances[v] = distances[u] + weight;
	  				}
	  			}
	  		}
	  		
	  		if(converged(distances, prev)){
	  			//std::cout << "Exited due to convergence" << std::endl;
	  			break;
	  		} else {
	  			//std::cout << "Graph did not converge" << std::endl;
	  		}
  		}
  		__itt_pause();
  		printGraphDistances();
	}

	void printGraphDistances() {
		std::cout << "Distances" << std::endl;
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
	
	serialBellmanFord bf(g);
	std::cout << "Graph size: " << g.size_nodes() << std::endl;
	bf.bellmanFord();

	return 0;
}
