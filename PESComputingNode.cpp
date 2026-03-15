#include "PESComputingNode.hpp"
#include <limits>
#include <random>


void PESComputingNode::processAddition(Edge& edge){
    processEdgeForPesInCocos(edge);
}


void PESComputingNode::processEdgeForPesInCocos(Edge& edge){

    VertexID src = edge.src;
    VertexID dst = edge.dst;

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
    
    count_triangles(edge);
    
    if (addition_coin_probability < prob_p) {
        addEdge(edge);
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

void PESComputingNode::addEdge(Edge& edge)
{
    VertexID src = edge.src;
    VertexID dst = edge.dst;

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
}


double PESComputingNode::getGlobalCnt(){

    globalCnt = (double)(delta_g * 1.0) / (prob_p * prob_q);
    return globalCnt;
}

std::unordered_map<VertexID, float> & PESComputingNode:: getLocalCnt(){
    return nodeToMotifs;
}

void PESComputingNode::count_triangles(Edge& edge)
{
    VertexID src = edge.src;
    VertexID dst = edge.dst;
    bool add = edge.add;

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
    std::unordered_map<VertexID, float>::iterator map_itr;


    while (itr != (src_itr->second).end()){
        VertexID neighbor = *itr;

        if (dst_set.find(neighbor) != dst_set.end()){
            count += 1;
            map_itr = nodeToMotifs.find(neighbor);
            if(map_itr == nodeToMotifs.end()){
                nodeToMotifs[neighbor] = weight;
            }else{
                nodeToMotifs[neighbor] = nodeToMotifs[neighbor] + weight; 
            }
        }

        itr++;
    }

    if (count > 0){
        double weight_sum = count * weight;

        map_itr = nodeToMotifs.find(src);
        if(map_itr == nodeToMotifs.end()){
            nodeToMotifs[src] = weight_sum;
        }else{
            nodeToMotifs[src] = nodeToMotifs[src] + weight_sum; 
        }

        map_itr = nodeToMotifs.find(dst);
        if(map_itr == nodeToMotifs.end()){
            nodeToMotifs[dst] = weight_sum;
        }else{
            nodeToMotifs[dst] = nodeToMotifs[dst] + weight_sum; 
        }

        globalCnt += weight_sum;
    }
}