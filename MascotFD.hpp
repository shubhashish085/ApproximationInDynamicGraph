#ifndef ADG_MASCOT_FD
#define ADG_MASCOT_FD

#include <unordered_map>
#include <unordered_set>
#include "graph.h"
#include "types.h"

class MascotFD {

public:

    std::unordered_map<VertexID, std::unordered_set<VertexID>> srcToDsts; // graph composed of the sampled edges
    std::unordered_map<VertexID, double> nodeToTriangles; // local triangle counts
    double globalTriangle = 0; // global triangle count

    ui k; // maximum number of samples
    double p; //edge sampling probability
    ui** samples; // sampled edges
    std::unordered_map<KeyID, ui> edgeToIndex; // edge to the index of cell that the edge is stored in

    bool lower_bound;

    MascotFD(ui memory_budget, double sample_probability, bool lowerbound) {

        k = memory_budget;
        p = sample_probability;
        lower_bound = lowerbound;
    }

    ~MascotFD() {
    
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