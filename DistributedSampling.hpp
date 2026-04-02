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

    //cocos
    int workerNum;
    double tolerancePlusOne;

    long* workerToLoad;
    long minLoad;
    double threshold;
    long minLoadWorker;

    NodeID* nodeToWorker;
    NodeID maxVId;
    VertexID capacity;

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

    ComputingNode(int workerNum, double tolerance, int k, unsigned int seed): 
    workerNum(workerNum), tolerancePlusOne(tolerance+1.0), workerToLoad(nullptr), minLoad(0), threshold(0.0), minLoadWorker(0),nodeToWorker(nullptr),  maxVId(128){
        
        srand(seed+time(NULL));
	    samples = new ui* [2];

        for(ui i = 0; i < 2; i++){
            samples[i] = new ui[k];
        }

        capacity = maxVId;
        workerToLoad = new long[workerNum];
        for (NodeID i = 0; i < workerNum; i++)
        {
            workerToLoad[i] = 0;
        }

        nodeToWorker = new NodeID[maxVId];
        for (NodeID i = 0; i < maxVId; i++)
        {
            nodeToWorker[i] = missingMId;
        }
    }

    ComputingNode(int k, unsigned int seed){
        srand(seed+time(NULL));
	    samples = new ui* [2];

        for(ui i = 0; i < 2; i++){
            samples[i] = new ui[k];
        }
    }

    ~ComputingNode(){
        if(workerToLoad != nullptr) {
            delete[] workerToLoad;
        }

        if(nodeToWorker != nullptr) {
            delete[] nodeToWorker;
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

    bool processEdgeForMasterInCocos(Edge &iEdge, NodeID &oDstMID1, NodeID &oDstMID2);
        
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