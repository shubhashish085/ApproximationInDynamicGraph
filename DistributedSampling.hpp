#ifndef ADG_DIST_SAMPLING_FD
#define ADG_DIST_SAMPLING_FD

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <random>
#include "graph.h"
#include "types.h"

class ComputingNode {

public:

    VertexID maxVertexId;

    std::unordered_map<VertexID, std::unordered_set<VertexID>> srcToDsts; // graph composed of the sampled edges
    std::unordered_map<VertexID, float> nodeToMotifs; // local triangle counts
    double globalCnt = 0; // global triangle count

    long long s = 0; // number of current samples
    int nb = 0; // number of uncompensated deletions
    int ng = 0; // number of uncompensate deletions

    ui k; // maximum number of samples
    double p; //edge sampling probability
    ui** samples; // sampled edges
    std::unordered_map<KeyID, ui> edgeToIndex; // edge to the index of cell that the edge is stored in

    bool lowerBound = true;

    ComputingNode(){}

    ComputingNode(int k, unsigned int seed){
        srand(seed+time(NULL));
	    samples = new ui* [2];

        for(ui i = 0; i < 2; i++){
            samples[i] = new ui[k];
        }
    }


	// Only for MPI
    bool processEdgeForMaster(Edge &iEdge){

        VertexID src = iEdge.src;
        VertexID dst = iEdge.dst;
        if (src > maxVertexId) {
            maxVertexId = src;
        }
        if (dst > maxVertexId) {
            maxVertexId = dst;
        }

        return true;
    }    

        
    void addEdge(Edge& edge);

    void deleteEdge(Edge& edge);

    void processAddition(Edge& edge);

    void processDeletion(Edge& edge);

    void processEdgeForWorker(Edge& edge);

    void processEdgeSquareForWorker(Edge& edge);

    void count_triangles(Edge& edge);

    void count_squares(Edge& edge);

    double getGlobalCnt();

	std::unordered_map<VertexID, float> & getLocalCnt();
};


#endif