#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>
#include <vector>
#include <set>
#include <algorithm>
#include <limits>
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
    std::vector<std::pair<VertexID, VertexID>> edge_list;
    //std::unordered_map<KeyID, int> edgeToIndex; 

public:

    std::unordered_map<VertexID, ui> node_to_triangles; // local triangle counts
    long long global_triangle_cnt = 0;
    long long global_square_cnt = 0;
    long long global_butterfly_cnt = 0;

    Graph() {

        vertices_count = 0;
        edges_count = 0;
        max_degree = 0;
        global_triangle_cnt = 0;
        global_square_cnt = 0;
        global_butterfly_cnt = 0;

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
    long long alt_count_exact_triangle();
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

        edge_list.push_back(std::make_pair(u, v));
        KeyID key = ((KeyID)u * std::numeric_limits<unsigned int>::max()) + v;

        //edgeToIndex.emplace(key, edge_list.size() - 1);

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

        ui set_intersection_length = get_nbr_set_intersection_count(u, v);
        global_triangle_cnt += set_intersection_length;
    }

    void add_edge_square(VertexID u, VertexID v){

        edge_list.push_back(std::make_pair(u, v));

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

        ui square_cnt = get_square_count(u, v);
        global_square_cnt += square_cnt;
    }

    void add_edge_butterfly(VertexID u, VertexID v){

        edge_list.push_back(std::make_pair(u, v));

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

        ui bfy_cnt = get_butterfly_count(u, v);
        global_butterfly_cnt += bfy_cnt;
    }



    bool delete_edge(VertexID u, VertexID v){

        std::set<VertexID> nbr_set;

        ui set_intersection_length = get_nbr_set_intersection_count(u, v);
        global_triangle_cnt -= set_intersection_length;

        if(adj_list.find(u) != adj_list.end()){
            nbr_set = adj_list[u];
            nbr_set.erase(v);
            if(nbr_set.empty()){
                 adj_list.erase(u);
            }
        }

        if(adj_list.find(v) != adj_list.end()){
            nbr_set = adj_list[v];
            nbr_set.erase(u);
            if(nbr_set.empty()){
                adj_list.erase(v);
            }
        }

        return true;
    }

    bool delete_edge_square(VertexID u, VertexID v){

        std::set<VertexID> nbr_set;
        ui set_intersection_length = 0;

        if(adj_list.find(u) != adj_list.end() && adj_list.find(v) != adj_list.end()){
            set_intersection_length = get_square_count(u, v);
        }

        global_square_cnt -= set_intersection_length;

        if(adj_list.find(u) != adj_list.end()){
            nbr_set = adj_list[u];
            nbr_set.erase(v);
            if(nbr_set.empty()){
                 adj_list.erase(u);
            }
        }

        if(adj_list.find(v) != adj_list.end()){
            nbr_set = adj_list[v];
            nbr_set.erase(u);
            if(nbr_set.empty()){
                adj_list.erase(v);
            }
        }

        return true;
    }


    bool delete_edge_butterfly(VertexID u, VertexID v){

        std::set<VertexID> nbr_set;
        ui set_intersection_length = 0;

        if(adj_list.find(u) != adj_list.end() && adj_list.find(v) != adj_list.end()){
            set_intersection_length = get_butterfly_count(u, v);
        }

        global_butterfly_cnt -= set_intersection_length;

        if(adj_list.find(u) != adj_list.end()){
            nbr_set = adj_list[u];
            nbr_set.erase(v);
            if(nbr_set.empty()){
                adj_list.erase(u);
            }
        }

        if(adj_list.find(v) != adj_list.end()){
            nbr_set = adj_list[v];
            nbr_set.erase(u);
            if(nbr_set.empty()){
                adj_list.erase(v);
            }
        }

        return true;
    }


    long long get_global_triangle_count(){
        return global_triangle_cnt;
    }

    long long get_global_square_count(){
        return global_square_cnt;
    }

    long long get_global_butterfly_count(){
        return global_butterfly_cnt;
    }

    ui get_square_count(VertexID u, VertexID v){

        ui square_count = 0;
        std::set<VertexID>::iterator itr, inner_itr;

        for (itr = adj_list[u].begin(); itr != adj_list[u].end(); itr++) {
            for(inner_itr = adj_list[*itr].begin(); inner_itr != adj_list[*itr].end(); inner_itr++){
                if(*inner_itr != u && adj_list[v].find(*inner_itr) != adj_list[v].end()){
                    square_count += 1;
                }                
            }
        }

        return square_count;
    }

    ui get_butterfly_count(VertexID u, VertexID v){

        ui butterfly_count = 0;

        std::set<VertexID>::iterator itr, inner_itr;

        for (itr = adj_list[u].begin(); itr != adj_list[u].end(); itr++) {
            for(inner_itr = adj_list[*itr].begin(); inner_itr != adj_list[*itr].end(); inner_itr++){
                if(*inner_itr != u && adj_list[v].find(*inner_itr) != adj_list[v].end()){
                    butterfly_count += 1;
                }                
            }
        }

        return butterfly_count;        
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