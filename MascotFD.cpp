#include <limits>
#include <random>
#include "MascotFD.hpp"

void MascotFD::processAddition(VertexID src, VertexID dst){
    processEdge(src, dst, true);
}

void MascotFD::processDeletion(VertexID src, VertexID dst){
    processEdge(src, dst, false);
}

void MascotFD::processEdge(VertexID src, VertexID dst, bool add){

    if (src == dst)
    { // ignore self loop
        return;
    }

    std::random_device rd;
    std::mt19937_64 eng(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    double addition_coin_probability = dis(eng);

    if (src > dst)
    {
        VertexID temp = src;
        src = dst;
        dst = temp;
    }
    
    count_triangles(src, dst, add); //count the added or deleted triangles

    if (add == false) { //deletion
        deleteEdge(src, dst);
    } else if (addition_coin_probability < p) {
        addEdge(src, dst);
        // s++; // the sum of sampled edges
    }

    return;

}

void MascotFD::addEdge(VertexID src, VertexID dst)
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

void MascotFD::deleteEdge(VertexID src, VertexID dst)
{

    ui sampleNum = edgeToIndex.size(), index;
    KeyID key = ((KeyID)src * std::numeric_limits<unsigned int>::max()) + dst;
    std::unordered_map<KeyID, ui>::iterator edge_idx_itr = edgeToIndex.find(key);
    if (edge_idx_itr != edgeToIndex.end())
    {
        index = edge_idx_itr->second;
        edgeToIndex.erase(edge_idx_itr);
    }

    std::unordered_map<VertexID, std::unordered_set<VertexID>>::iterator itr = srcToDsts.find(src);
    if (itr != srcToDsts.end())
    {
        (itr->second).erase(dst);
        if ((itr->second).empty())
        {
            srcToDsts.erase(itr);
        }
    }

    itr = srcToDsts.find(dst);
    if (itr != srcToDsts.end())
    {
        (itr->second).erase(src);
        if ((itr->second).empty())
        {
            srcToDsts.erase(itr);
        }
    }

    if (index < sampleNum - 1)
    {
        int newSrc = samples[0][index] = samples[0][sampleNum - 1];
        int newDst = samples[1][index] = samples[1][sampleNum - 1];
        long newKey = ((long)newSrc * std::numeric_limits<unsigned int>::max()) + newDst;
        edgeToIndex.emplace(newKey, index);
    }
}

double MascotFD::getGlobalTriangle(){
    return globalTriangle;
}

std::unordered_map<VertexID, double> MascotFD::getLocalTriangle(){
    return nodeToTriangles;
}

void MascotFD::count_triangles(VertexID src, VertexID dst, bool add)
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

    double weight = 1 / (p * p);
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