#ifndef ADG_THINKD_FD
#define ADG_THINKD_FD

#include <unordered_map>
#include <unordered_set>
#include "graph.h"
#include "types.h"

class ThinkDFD {

public:

    std::unordered_map<VertexID, std::unordered_set<VertexID>> srcToDsts; // graph composed of the sampled edges
    std::unordered_map<VertexID, double> nodeToTriangles; // local triangle counts
    std::unordered_map<VertexID, double> nodeToSquares; // local square counts
    std::unordered_map<VertexID, double> nodeToButterflies; // local butterfly counts
    double globalTriangle = 0; // global triangle count
    double globalSquare = 0; // global square count
    double globalButterfly = 0; // global butterfly count

    long long s = 0; // number of current samples
    int nb = 0; // number of uncompensated deletions
    int ng = 0; // number of uncompensate deletions

    ui k; // maximum number of samples
    ui** samples; // sampled edges
    std::unordered_map<KeyID, ui> edgeToIndex; // edge to the index of cell that the edge is stored in

    bool lowerBound;

    ThinkDFD(ui memory_budget, bool lower_bound) {

        k = memory_budget;
        lowerBound = lower_bound;
        samples = new ui* [2];
        for (ui i = 0; i < 2; i++){
            samples[i] = new ui[k];
        }
    }

    ~ThinkDFD() {
    }

    void addEdge(VertexID src, VertexID dst);

    void deleteEdge(VertexID src, VertexID dst);

    void processAddition(VertexID src, VertexID dst);

    void processDeletion(VertexID src, VertexID dst);

    void processEdge(VertexID src, VertexID dst, bool add);

    void processEdgeSquare(VertexID src, VertexID dest, bool add);

    void processEdgeButterfly(VertexID src, VertexID dest, bool add);

    void count_triangles(VertexID src, VertexID dst, bool add);

    void count_squares(VertexID src, VertexID dst, bool add);

    void count_butterfly(VertexID src, VertexID dst, bool add);

    double getGlobalTriangle();

    double getGlobalSquare();

    double getGlobalButterfly();

    std::unordered_map<VertexID, double> getLocalTriangle();

    std::unordered_map<VertexID, double> getLocalSquare();
};


#endif