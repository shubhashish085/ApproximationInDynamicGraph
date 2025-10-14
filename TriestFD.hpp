#ifndef ADG_TRIEST_FD
#define ADG_TRIEST_FD

#include <unordered_map>
#include <unordered_set>
#include "graph.h"
#include "types.h"

class TriestFD {

public:

    std::unordered_map<VertexID, std::unordered_set<VertexID>> srcToDsts; // graph composed of the sampled edges
    std::unordered_map<VertexID, double> nodeToTriangles; // local triangle counts
    double globalTriangle = 0; // global triangle count

    long long s = 0; // number of current samples
    ui nb = 0; // number of uncompensated deletions
    ui ng = 0; // number of uncompensate deletions

    ui k; // maximum number of samples
    ui** samples; // sampled edges
    std::unordered_map<KeyID, ui> edgeToIndex; // edge to the index of cell that the edge is stored in

    bool lowerBound;

    TriestFD(ui memory_budget, bool lower_bound) {

        k = memory_budget;
        lowerBound = lower_bound;
    }

    ~TriestFD() {
    }

    void addEdge(VertexID src, VertexID dst);

    void deleteEdge(VertexID src, VertexID dst);

    void processAddition(VertexID src, VertexID dst);

    void processDeletion(VertexID src, VertexID dst);

    void processEdge(VertexID src, VertexID dst, bool add);

    void count_triangles(VertexID src, VertexID dst, bool add);

    double getGlobalTriangle();

    std::unordered_map<VertexID, double> getLocalTriangle();
};


#endif