#include "PES.hpp"
#include <limits>
#include <random>


void PES::processAddition(VertexID src, VertexID dst){
    processEdge(src, dst, true);
}


void PES::processEdge(VertexID src, VertexID dst, bool add){

    if (src == dst)
    { // ignore self loop
        return;
    }

    std::random_device rd;
    std::mt19937_64 eng(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    double addition_coin_probability = dis(eng), replacement_coin_probability;

    if (src > dst)
    {
        VertexID temp = src;
        src = dst;
        dst = temp;
    }
    
    count_triangles(src, dst, add);
    
    if (addition_coin_probability < prob_p) {
        addEdge(src, dst);
    }

    auto it = wedge_pool.find(std::make_pair(src, dst));
    ui wedge_idx;

    while(it != wedge_pool.end()){
        wedge_idx = (it->second).second;
        std::get<3>(wedge_list[wedge_idx]) = true;
        delta_g += 1.0;
        it++;
    }

    std::unordered_set<VertexID> adj_set;
    std::tuple<VertexID, VertexID, VertexID, bool> wedge_tuple, new_wedge_tuple;

    std::mt19937_64 rd_replace_eng(rd());
    std::uniform_int_distribution<int> rd_replace_dis(0, candidate_wedge_count);

    if(srcToDsts.find(src) != srcToDsts.end()){
        adj_set = srcToDsts[src];
        for(auto itr = adj_set.begin(); itr != adj_set.end(); itr++){
            candidate_wedge_count += 1;
            if(candidate_wedge_count < wedge_pool_size){
                addWedge(dst, src, *itr);
            }else{
                prob_q = (double) (wedge_pool_size * 1.0) / candidate_wedge_count;
                
                replacement_coin_probability = dis(eng);
                if(replacement_coin_probability <= prob_q){
                    wedge_idx = rd_replace_dis(rd_replace_eng);
                    wedge_tuple = wedge_list[wedge_idx];
                    if(std::get<3>(wedge_tuple)){
                        delta_g -= 1;
                    }
                    new_wedge_tuple = std::make_tuple(dst, src, *itr, false);
                    replaceWedge(wedge_idx, new_wedge_tuple);
                }

            }
        }
    }

    if(srcToDsts.find(dst) != srcToDsts.end()){
        adj_set = srcToDsts[dst];
        for(auto itr = adj_set.begin(); itr != adj_set.end(); itr++){
            candidate_wedge_count += 1;
            if(candidate_wedge_count < wedge_pool_size){
                addWedge(src, dst, *itr);
            }else{
                prob_q = (double) (wedge_pool_size * 1.0) / candidate_wedge_count;

                replacement_coin_probability = dis(eng);
                if(replacement_coin_probability <= prob_q){
                    wedge_idx = rd_replace_dis(rd_replace_eng);
                    wedge_tuple = wedge_list[wedge_idx];
                    if(std::get<3>(wedge_tuple)){
                        delta_g -= 1;
                    }
                    new_wedge_tuple = std::make_tuple(src, dst, *itr, false);
                    replaceWedge(wedge_idx, new_wedge_tuple);
                }
            }
        }
    }


    return;

}

void PES::addEdge(VertexID src, VertexID dst)
{

    ui sampleNum = edgeToIndex.size();
    samples[0][sampleNum] = src;
    samples[1][sampleNum] = dst;
    //std::cout << "Adding Edge .... : " << sampleNum << std::endl;
    KeyID key = ((KeyID)src * std::numeric_limits<unsigned int>::max()) + dst;

    edgeToIndex.emplace(key, sampleNum);
    if (srcToDsts.find(src) == srcToDsts.end())
    {
        std::unordered_set<VertexID> dst_set;
        srcToDsts.emplace(src, dst_set);
    }
    std::unordered_map<VertexID, std::unordered_set<VertexID>>::iterator itr = srcToDsts.find(src);
    (itr->second).insert(dst);

    if (srcToDsts.find(dst) == srcToDsts.end())
    {
        std::unordered_set<VertexID> src_set;
        srcToDsts.emplace(dst, src_set);
    }
    itr = srcToDsts.find(dst);
    (itr->second).insert(src);
    //std::cout << "Adding Edge Finished : " << sampleNum << std::endl;

}


double PES::getGlobalTriangle(){

    globalTriangle = (double)(delta_g * 1.0) / (prob_p * prob_q);
    return globalTriangle;
}

std::unordered_map<VertexID, double> PES::getLocalTriangle(){
    return nodeToTriangles;
}

void PES::count_triangles(VertexID src, VertexID dst, bool add)
{

    // if this edge has a new node, there cannot be any triangles
    if (srcToDsts.find(src) == srcToDsts.end() || srcToDsts.find(dst) == srcToDsts.end()){
        return;
    }

    std::unordered_map<VertexID, std::unordered_set<VertexID>>::iterator src_itr = srcToDsts.find(src);
    std::unordered_map<VertexID, std::unordered_set<VertexID>>::iterator dst_itr = srcToDsts.find(dst);

    if ((src_itr->second).size() > (dst_itr->second).size())
    {
        std::unordered_map<VertexID, std::unordered_set<VertexID>>::iterator temp = src_itr;
        src_itr = dst_itr;
        dst_itr = temp;
    }

    double weight = 1 / (prob_p * prob_p);
    if (!add) {
        weight = (-1) * weight;
    }

    double count = 0;
    std::unordered_set<VertexID>::iterator itr = (src_itr->second).begin();
    std::unordered_set<VertexID> dst_set = dst_itr->second;
    std::unordered_map<VertexID, double>::iterator map_itr;


    while (itr != (src_itr->second).end()){
        VertexID neighbor = *itr;

        if (dst_set.find(neighbor) != dst_set.end()){
            count += 1;
            map_itr = nodeToTriangles.find(neighbor);
            if(map_itr == nodeToTriangles.end()){
                nodeToTriangles[neighbor] = weight;
            }else{
                nodeToTriangles[neighbor] = nodeToTriangles[neighbor] + weight; 
            }
        }

        itr++;
    }

    if (count > 0){
        double weight_sum = count * weight;

        map_itr = nodeToTriangles.find(src);
        if(map_itr == nodeToTriangles.end()){
            nodeToTriangles[src] = weight_sum;
        }else{
            nodeToTriangles[src] = nodeToTriangles[src] + weight_sum; 
        }

        map_itr = nodeToTriangles.find(dst);
        if(map_itr == nodeToTriangles.end()){
            nodeToTriangles[dst] = weight_sum;
        }else{
            nodeToTriangles[dst] = nodeToTriangles[dst] + weight_sum; 
        }

        globalTriangle += weight_sum;
    }
}