#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>
#include <vector>
#include <set>
#include <algorithm>
#include "types.h"

class Graph{

private:

    ui vertices_count;
    ui edges_count;
    ui max_degree;

    ui* degrees;
    ui* offsets;
    VertexID * neighbors;

    std::unordered_map<VertexID, std::set<VertexID>> adj_list;

public:

    Graph() {

        vertices_count = 0;
        edges_count = 0;
        max_degree = 0;

        offsets = NULL;
        neighbors = NULL;
    }

    ~Graph() {
        delete[] offsets;
        delete[] neighbors;
    }


    void loadGraphFromFile(const std::string& file_path);
    void printGraphMetaData();
    void printGraphDegreeData();
    long long count_exact_triangle();
    long long count_exact_butterfly();
    long long count_exact_square();


    const ui* getOffsets() const {
        return offsets;
    }

    const ui* getNeighbors() const {
        return neighbors;
    }

    const ui getVerticesCount() const {
        return vertices_count;
    }

    const ui getEdgesCount() const {
        return edges_count;
    }

    const ui getVertexDegree(const VertexID id) const {
        return offsets[id + 1] - offsets[id];
    }

    const ui getGraphMaxDegree() const {
        return max_degree;
    }

    ui * getVertexNeighbors(const VertexID id, ui& count) const {
        count = offsets[id + 1] - offsets[id]; // used for neighbor count
        return neighbors + offsets[id];
    }

    void getNeighborCount(const VertexID id, ui& count) const {
        count = offsets[id + 1] - offsets[id];
    }

    bool checkEdgeExistence(VertexID u, VertexID v) const {
        if (getVertexDegree(u) < getVertexDegree(v)) {
            std::swap(u, v);
        }
        ui count = 0;
        const VertexID* neighbors =  getVertexNeighbors(v, count);

        int begin = 0;
        int end = count - 1;
        while (begin <= end) {
            int mid = begin + ((end - begin) >> 1);
            if (neighbors[mid] == u) {
                return true;
            }
            else if (neighbors[mid] > u)
                end = mid - 1;
            else
                begin = mid + 1;
        }

        return false;
    }


    void add_edge(VertexID u, VertexID v) {
        
        if(adj_list.find(u) == adj_list.end()){
            std::set<VertexID> u_nbr;
            u_nbr.insert(v);
            adj_list.insert({u, u_nbr});
        }else{
            adj_list[u].insert(v);
        }


        if(adj_list.find(v) == adj_list.end()){
            std::set<VertexID> v_nbr;
            v_nbr.insert(u);
            adj_list.insert({v, v_nbr});
        }else{
            adj_list[v].insert(u);
        }
    }

    ui get_nbr_set_intersection_count(VertexID u, VertexID v){

        std::set<VertexID> intersection_set;

        std::set_intersection(adj_list[u].begin(), adj_list[u].end(), adj_list[v].begin(), adj_list[v].end(), std::inserter(intersection_set, intersection_set.begin()));

        ui intersection_set_size =  (ui)(intersection_set.size());

        return intersection_set_size;
    }


    void printGraphData(){

        std::cout << " >>>>> Degree <<<<<<" << std::endl;

        for(ui i = 0; i < vertices_count; i++){
            std::cout << degrees[i] << " ";
        }

        std::cout << std::endl;

        std::cout << " >>>>> Offset <<<<<<" << std::endl;

        for(ui i = 0; i < vertices_count + 1; i++){
            std::cout << offsets[i] << " ";
        }

        std::cout << std::endl;
    }

};


#endif