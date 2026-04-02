#ifndef ADG_PES
#define ADG_PES

#include <map>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include "graph.h"
#include "types.h"

struct pair_hash_pes {

    static inline size_t hash_vertex(VertexID u) {
        size_t x = u;
        x ^= x >> 33;
        x *= 0xff51afd7ed558ccdULL;
        x ^= x >> 33;
        x *= 0xc4ceb9fe1a85ec53ULL;
        x ^= x >> 33;
        return x;
    }

    std::size_t operator()(const std::pair<VertexID, VertexID>& p) const {
        std::size_t h1 = hash_vertex(p.first);
        std::size_t h2 = hash_vertex(p.second);
        return h1 ^ (h2 << 1);
    }
};


class PES {

public:

    std::unordered_multimap<std::pair<VertexID, VertexID>, std::pair<VertexID, ui>, pair_hash_pes> wedge_pool;
    std::vector<std::tuple<VertexID, VertexID, VertexID, bool>> wedge_list;

    std::unordered_map<VertexID, std::unordered_set<VertexID>> srcToDsts;
    std::unordered_map<VertexID, double> nodeToTriangles;
    double globalTriangle = 0;

    ui k, wedge_pool_size;
    double delta_g = 0.0;
    ui candidate_wedge_count = 0;
    double prob_p, prob_q;
    ui** samples;
    std::unordered_map<KeyID, ui> edgeToIndex;


    PES(ui memory_budget, double sample_probability, ui pool_size) {

        k = memory_budget;
        prob_p = sample_probability;
        prob_q = sample_probability;
        wedge_pool_size = pool_size;
        samples = new ui* [2];
        for (ui i = 0; i < 2; i++){
            samples[i] = new ui[k];
        }
    }

    ~PES() {
    
    }    
    
    void addEdge(VertexID src, VertexID dst);

    void addWedge(VertexID src, VertexID mid_vtx, VertexID dst){

        if(src > dst){
            VertexID tmp = dst;
            dst = src;
            src = tmp;
        }

        wedge_list.push_back(std::make_tuple(src, mid_vtx, dst, false));
        std::pair<VertexID, VertexID> key_pair = std::make_pair(src, dst);
        std::pair<VertexID, ui> value_pair = std::make_pair(mid_vtx, candidate_wedge_count);

        wedge_pool.insert({key_pair, value_pair});
        candidate_wedge_count++;
    }

    void replaceWedge(ui index, std::tuple<VertexID, VertexID, VertexID, bool> new_wedge){

        std::tuple<VertexID, VertexID, VertexID, bool> wedge_tuple = wedge_list[index];

        VertexID src = std::get<0>(wedge_tuple);
        VertexID mid_vertex = std::get<1>(wedge_tuple);
        VertexID dst = std::get<2>(wedge_tuple);

        bool flag = false; 
        
        if(wedge_pool.find(std::make_pair(src, dst)) != wedge_pool.end()){

            auto it = wedge_pool.begin();
            while (it != wedge_pool.end()) {
                
                while (it != wedge_pool.end() && (it->first).first == src && (it->first).second == dst) {

                    if((it->second).first == mid_vertex){
                        flag = true;
                        wedge_pool.erase(it);
                        break;
                    }

                    if(flag){
                        break;
                    }
            
                    it++;
                }
            }

        }

        wedge_list[index] = new_wedge;
        wedge_pool.insert({std::make_pair(std::get<0>(new_wedge), std::get<2>(new_wedge)), std::make_pair(std::get<1>(new_wedge), index)});         

    }

    void processAddition(VertexID src, VertexID dst);

    void processEdge(VertexID src, VertexID dst, bool add);

    void count_triangles(VertexID src, VertexID dst, bool add);

    double getGlobalTriangle();

    std::unordered_map<VertexID, double> getLocalTriangle();
};


#endif